#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NAME_LEN 1000
#define NUM_THREADS 16
#define MAXCONN 7
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

long int *data;
long int *temp;
long int CURR_THREADS;
long int NUM_BLK;
long int SIZE;
long int FSIZE;
long int RSIZE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];


double start_time, end_time, orig_time;
struct timeval t;

void set_time(int timer) {
	gettimeofday(&t, NULL);
	if (timer == 0) {
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 1) {
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 2) {
		orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	}
}
void read_long(int sockfd_client, char *num) {
	unsigned int size = sizeof(long int);
	int rlen = 0;
	int ret;

	while (rlen < size) {
		if ((ret = read(sockfd_client, (num + rlen), size - rlen)) == -1) {
			perror("read_long");
			exit(1);
		}

		if (ret <= 0) {
			perror("socket closed before consumption");
			exit(1);
		}
		rlen += ret;
	}
}

void tokenize(char *buffer_temp, char *exec_args[]) {
	int counter = 0;
	char *token = strtok(buffer_temp, " ");
	while (token != NULL) {
		exec_args[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}
	exec_args[counter] = NULL;
}

bool is_sorted(long int start, long int end) {
	int err_ct = 0;
	for (long int i = start; i <= end - 1; i++) {
		if (data[i] > data[i + 1]) {
			printf("ERROR : %ld\t%ld\t%ld\t%ld\n", data[i], data[i + 1], i, i + 1);
			err_ct++;
		}
	}
	if (err_ct > 0) {
		printf("err_ct = %d\n", err_ct);
		return false;
	}
	return true;
}

static __inline__ void sort8(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);
	SWAP(4, 5, lo);
	SWAP(6, 7, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(4, 6, lo);
	SWAP(5, 7, lo);

	SWAP(1, 2, lo);
	SWAP(5, 6, lo);
	SWAP(0, 4, lo);
	SWAP(3, 7, lo);

	SWAP(1, 5, lo);
	SWAP(2, 6, lo);

	SWAP(1, 4, lo);
	SWAP(3, 6, lo);

	SWAP(2, 4, lo);
	SWAP(3, 5, lo);

	SWAP(3, 4, lo);
}
static __inline__ void sort7(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);

	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(4, 6, lo);

	SWAP(0, 1, lo);
	SWAP(4, 5, lo);
	SWAP(2, 6, lo);

	SWAP(0, 4, lo);
	SWAP(1, 5, lo);

	SWAP(0, 3, lo);
	SWAP(2, 5, lo);
	SWAP(1, 3, lo);
	SWAP(2, 4, lo);
	SWAP(2, 3, lo);
}

static __inline__ void sort6(long int * data, long int lo) {
	//Parallelizable
	SWAP(1, 2, lo);
	SWAP(4, 5, lo);

	//Parallelizable
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);

	//Parallelizable
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 5, lo);

	//Parallelizable
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);

	//Parallelizable
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);

	//Parallelizable
	SWAP(2, 3, lo);
}

static __inline__ void sort5(long int * data, long int lo) {

	SWAP(0, 1, lo);
	SWAP(3, 4, lo);

	SWAP(2, 4, lo);

	SWAP(2, 3, lo);
	SWAP(1, 4, lo);

	SWAP(0, 3, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);

	SWAP(1, 2, lo);
}

static __inline__ void sort4(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);

	SWAP(1, 2, lo);
}

static __inline__ void sort3(long int * data, long int lo) {
	SWAP(1, 2, lo);

	SWAP(0, 2, lo);

	SWAP(0, 1, lo);
}

static __inline__ void sort2(long int * data, long int lo) {
	SWAP(0, 1, lo);
}

void merge(long int lo, long int mid, long int hi) {
	long int i, j, k;
//	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
	for (long int i = lo; i <= hi; i++) {
		temp[i] = data[i];
	}
	i = lo, j = mid + 1;

	for (k = lo; k <= hi; k++) {
		if (i > mid)
			data[k] = temp[j++];
		else if (j > hi)
			data[k] = temp[i++];
		else if (temp[i] < temp[j])
			data[k] = temp[i++];
		else
			data[k] = temp[j++];
	}
}

void mergesort(long int lo, long int hi) {
	if (hi <= lo)
		return;
	long int num = hi - lo + 1;
	if (num <= 8) {
		switch (num) {
		case 0:
		case 1:
			return;
		case 2:
			sort2(data, lo);
			return;
		case 3:
			sort3(data, lo);
			return;
		case 4:
			sort4(data, lo);
			return;
		case 5:
			sort5(data, lo);
			return;
		case 6:
			sort6(data, lo);
			return;
		case 7:
			sort7(data, lo);
			return;
//		case 8:
//			sort8(data, lo);
//			return;
		}
	}

	long int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
//	if (!(data[mid] < data[mid + 1])) {
//		Merge only if data is sorted
	merge(lo, mid, hi);
//	}
}

void *mergesort_caller(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
	long int start = myid * num_ele;
	long int end = (((myid + 1) * num_ele - 1) < (SIZE - 1)) ? ((myid + 1) * num_ele - 1) : (SIZE - 1);
	mergesort(start, end);
}

void *k_way_merger_single(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
	long int start1 = myid * num_ele;
	long int end1 = myid * num_ele + num_ele / 2 - 1;
	long int start2 = myid * num_ele + num_ele / 2;
	long int end2 = (myid + 1) * num_ele - 1;
	if (end2 >= SIZE) {
		end2 = SIZE - 1;
	}

	long int curr1 = start1, curr2 = start2;

//	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);
	long int i = start1;
	while (curr1 <= end1 && curr2 <= end2) {
		if (data[curr1] <= data[curr2])
			temp[i++] = data[curr1++];
		else
			temp[i++] = data[curr2++];
	}

	while (curr1 <= end1) {
		temp[i++] = data[curr1++];
	}

	while (curr2 <= end2) {
		temp[i++] = data[curr2++];
	}

	for (long int i = start1; i <= end2; i++)
		data[i] = temp[i];
}

void k_way_single() {
	CURR_THREADS = NUM_THREADS >> 1;
	while (CURR_THREADS != 0) {
		for (int i = 0; i < CURR_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &k_way_merger_single, &myid[i]);
		}

		for (int i = 0; i < CURR_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}

		CURR_THREADS = CURR_THREADS >> 1;
	}

}

int main(int argc, char **argv) {

	if (argc < 4) {
		printf("Usage ./sort_server large_file_name current_ip_address machinefile\n");
		exit(1);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Get the 7 machines from machinefile*/
	FILE *file = fopen(argv[3], "r");
	char mac_list[MAXCONN][1000];
	int num_machines = 0;
	char buffer_machines[NAME_LEN];
	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
		num_machines++;
	}

	if (num_machines < MAXCONN) {
		printf("Ensure that the file exists and there are exactly 7 machines which are active with 8GB memory free\n");
		exit(1);
	}
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < MAXCONN; i++) {
		/*Read the machines from machine file*/
		if (fscanf(file, "%s\n", mac_list[i]) == 0) {
			printf("Error reading machinefile = %s\n", argv[3]);
			exit(1);
		} else {
			printf("Machine i = %s\n", mac_list[i]);
		}

	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Creating sockets from here*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in saddr_server, saddr_client;
	int sfd_server[MAXCONN];
	int sfd_client[MAXCONN];
	socklen_t len = sizeof(saddr_server);

	for (int i = 0; i < MAXCONN; i++) {
		saddr_server.sin_addr.s_addr = INADDR_ANY;
		saddr_server.sin_family = AF_INET;
		saddr_server.sin_port = htons((short) (5555 + i));

		printf("Creating socket for %d\n", i);
		if ((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			exit(1);
		}

		printf("Binding socket for %d\n", i);
		if (bind(sockfd_server, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) < 0) {
			perror("bind");
			exit(1);
		}

		printf("Getting name socket for %d\n", i);
		if (getsockname(sockfd_server, (struct sockaddr *) &saddr_server, &len) < 0) {
			perror("getsockname");
			exit(1);
		}
		sfd_server[i] = sockfd_server;
		printf("Listening to socket %d\n", i);
		if (listen(sfd_server[i], 1) < 0) {
			perror("listen");
			exit(1);
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Sending files to all the machines*/
	char buffer_temp[NAME_LEN];
	char *exec_args[NAME_LEN];

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
			long int skip = i * 8000000000l;
//
//			sprintf(buffer_temp, "/bin/dd if=%s bs=32M  iflag=skip_bytes,count_bytes skip=%ld count=8000000000 | ssh %s 'cat > temp'", argv[1], skip, (i + 2) , mac_list[i]);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");

			sprintf(buffer_temp, "scp sort_client.c %s:", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			sprintf(buffer_temp, "ssh %s gcc -o sort_client sort_client.c "
					"-march=native -Ofast -std=c99 -lm -pedantic "
					"-pthread -fopenmp -funroll-loops", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			sprintf(buffer_temp, "/usr/bin/ssh %s ./sort_client %s %d", mac_list[i], argv[2], (5555 + i));
			printf("%s\n", buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
			return 0;
		}
	}
	while (wait(NULL) > 0)
		;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Accepting the connection here*/

	for (int i = 0; i < MAXCONN; i++) {
		if ((sockfd_client = accept(sfd_server[i], (struct sockaddr *) (&saddr_client), &len)) < 0) {
			perror("accept");
			exit(1);
		}

		sfd_client[i] = sockfd_client;
		printf("client[%d] is remote machine = %s, port = %x, %x.\n", i, inet_ntoa(saddr_client.sin_addr), saddr_client.sin_port, ntohs(saddr_client.sin_port));
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 1 STARTED*/
	char outfile[100];

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 2 STARTED*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Time to read and merge data :)*/
	FILE *final = fopen("answer", "w+");
	long int num = -5;
//	for (int i = 0; i < 21; i++) {
//		read_long(sfd_client[i%MAXCONN], (char *) &num);
//		printf("number = %d  = %ld\n", i, num);
//	}

	int all_count = 0;

	for (int i = 0; i < MAXCONN; i++) {
		sprintf(buffer_temp, "ssh %s cat comp_and_ran", mac_list[i]);
		printf("%s\n", buffer_temp);

		if (system(buffer_temp) == -1)
			perror("System");
	}

	for (int i = 0; i < MAXCONN; i++) {
		sprintf(buffer_temp, "ssh %s rm -f comp_and_ran", mac_list[i]);
		printf("%s\n", buffer_temp);

		if (system(buffer_temp) == -1)
			perror("System");
	}

	return 0;

}
