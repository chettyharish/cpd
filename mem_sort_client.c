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
FILE *log_file;

#define SOCKET_BLK 100000000
#define BUFFERSIZ 8192
#define NAME_LEN 1000
#define NUM_THREADS 16
#define MAXCONN 7
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

struct timeval t;
long int *data;
long int *temp;
const long int NUM_BLK = 2;
const long int ELE_PER_PC = 1000000000;
long int CURR_THREADS;
long int SIZE;
long int FSIZE;
long int RSIZE;
long int NUM_ELE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];

static __inline__ void sort2(long int * data, long int lo) {
	SWAP(0, 1, lo);
}

static __inline__ void sort3(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(0, 2, lo);
	SWAP(0, 1, lo);
}

static __inline__ void sort4(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);
	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(1, 2, lo);
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
static __inline__ void sort6(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(4, 5, lo);
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 5, lo);
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);
	SWAP(2, 3, lo);
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

static __inline__ void sort9(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);
	SWAP(4, 5, lo);
	SWAP(7, 8, lo);
	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(6, 8, lo);
	SWAP(1, 2, lo);
	SWAP(6, 7, lo);
	SWAP(5, 8, lo);
	SWAP(4, 7, lo);
	SWAP(3, 8, lo);
	SWAP(4, 6, lo);
	SWAP(5, 7, lo);
	SWAP(5, 6, lo);
	SWAP(2, 7, lo);
	SWAP(0, 5, lo);
	SWAP(1, 6, lo);
	SWAP(3, 7, lo);
	SWAP(0, 4, lo);
	SWAP(1, 5, lo);
	SWAP(3, 6, lo);
	SWAP(0, 4, lo);
	SWAP(1, 5, lo);
	SWAP(3, 6, lo)
	SWAP(1, 4, lo);
	SWAP(2, 5, lo);
	SWAP(2, 4, lo);
	SWAP(3, 5, lo);
	SWAP(3, 4, lo);
}

static __inline__ void sort10(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);
	SWAP(8, 9, lo);
	SWAP(2, 4, lo);
	SWAP(7, 9, lo);
	SWAP(2, 3, lo);
	SWAP(1, 4, lo);
	SWAP(7, 8, lo);
	SWAP(6, 9, lo);
	SWAP(0, 3, lo);
	SWAP(5, 8, lo);
	SWAP(4, 9, lo);
	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(5, 7, lo);
	SWAP(6, 8, lo);
	SWAP(1, 2, lo);
	SWAP(6, 7, lo);
	SWAP(0, 5, lo);
	SWAP(3, 8, lo);
	SWAP(1, 6, lo);
	SWAP(2, 7, lo);
	SWAP(4, 8, lo);
	SWAP(1, 5, lo);
	SWAP(3, 7, lo);
	SWAP(4, 7, lo);
	SWAP(2, 5, lo);
	SWAP(3, 6, lo);
	SWAP(4, 6, lo);
	SWAP(3, 5, lo);
	SWAP(4, 5, lo);
}
static __inline__ void sort11(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(6, 7, lo);
	SWAP(9, 10, lo);
	SWAP(2, 4, lo);
	SWAP(5, 7, lo);
	SWAP(8, 10, lo);
	SWAP(2, 3, lo);
	SWAP(1, 4, lo);
	SWAP(5, 6, lo);
	SWAP(8, 9, lo);
	SWAP(7, 10, lo);
	SWAP(0, 3, lo);
	SWAP(5, 8, lo);
	SWAP(6, 9, lo);
	SWAP(4, 10, lo);
	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(7, 9, lo);
	SWAP(6, 8, lo);
	SWAP(1, 2, lo);
	SWAP(7, 8, lo);
	SWAP(0, 6, lo);
	SWAP(3, 9, lo);
	SWAP(0, 5, lo);
	SWAP(1, 7, lo);
	SWAP(2, 8, lo);
	SWAP(4, 9, lo);
	SWAP(1, 6, lo);
	SWAP(3, 8, lo);
	SWAP(1, 5, lo);
	SWAP(4, 8, lo);
	SWAP(3, 6, lo);
	SWAP(2, 5, lo);
	SWAP(4, 7, lo);
	SWAP(4, 6, lo);
	SWAP(3, 5, lo);
	SWAP(4, 5, lo);
}

static __inline__ void sort12(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(4, 5, lo);
	SWAP(7, 8, lo);
	SWAP(10, 11, lo);
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(6, 8, lo);
	SWAP(9, 11, lo);
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 5, lo);
	SWAP(6, 7, lo);
	SWAP(9, 10, lo);
	SWAP(8, 11, lo);
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);
	SWAP(6, 9, lo);
	SWAP(7, 10, lo);
	SWAP(5, 11, lo);
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);
	SWAP(8, 10, lo);
	SWAP(7, 9, lo);
	SWAP(0, 6, lo);
	SWAP(2, 3, lo);
	SWAP(8, 9, lo);
	SWAP(1, 7, lo);
	SWAP(4, 10, lo);
	SWAP(2, 8, lo);
	SWAP(1, 6, lo);
	SWAP(3, 9, lo);
	SWAP(5, 10, lo);
	SWAP(2, 7, lo);
	SWAP(4, 9, lo);
	SWAP(2, 6, lo);
	SWAP(5, 9, lo);
	SWAP(4, 7, lo);
	SWAP(3, 6, lo);
	SWAP(5, 8, lo);
	SWAP(5, 7, lo);
	SWAP(4, 6, lo);
	SWAP(5, 6, lo);
}

static __inline__ void sort13(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(4, 5, lo);
	SWAP(7, 8, lo);
	SWAP(9, 10, lo);
	SWAP(11, 12, lo);
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(6, 8, lo);
	SWAP(9, 11, lo);
	SWAP(10, 12, lo);
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 5, lo);
	SWAP(6, 7, lo);
	SWAP(10, 11, lo);
	SWAP(8, 12, lo);
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);
	SWAP(6, 10, lo);
	SWAP(7, 11, lo);
	SWAP(5, 12, lo);
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);
	SWAP(6, 9, lo);
	SWAP(8, 11, lo);
	SWAP(2, 3, lo);
	SWAP(7, 9, lo);
	SWAP(8, 10, lo);
	SWAP(4, 11, lo);
	SWAP(8, 9, lo);
	SWAP(0, 7, lo);
	SWAP(3, 10, lo);
	SWAP(5, 11, lo);
	SWAP(0, 6, lo);
	SWAP(1, 8, lo);
	SWAP(2, 9, lo);
	SWAP(4, 10, lo);
	SWAP(2, 8, lo);
	SWAP(1, 6, lo);
	SWAP(5, 10, lo);
	SWAP(2, 7, lo);
	SWAP(4, 8, lo);
	SWAP(5, 9, lo);
	SWAP(2, 6, lo);
	SWAP(3, 7, lo);
	SWAP(5, 8, lo);
	SWAP(3, 6, lo);
	SWAP(5, 7, lo);
	SWAP(4, 6, lo);
	SWAP(5, 6, lo);
}

static __inline__ void sort14(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);
	SWAP(8, 9, lo);
	SWAP(10, 11, lo);
	SWAP(12, 13, lo);
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(4, 6, lo);
	SWAP(7, 9, lo);
	SWAP(10, 12, lo);
	SWAP(11, 13, lo);
	SWAP(0, 1, lo);
	SWAP(4, 5, lo);
	SWAP(2, 6, lo);
	SWAP(7, 8, lo);
	SWAP(11, 12, lo);
	SWAP(9, 13, lo);
	SWAP(0, 4, lo);
	SWAP(1, 5, lo);
	SWAP(7, 11, lo);
	SWAP(8, 12, lo);
	SWAP(6, 13, lo);
	SWAP(0, 3, lo);
	SWAP(2, 5, lo);
	SWAP(7, 10, lo);
	SWAP(9, 12, lo);
	SWAP(1, 3, lo);
	SWAP(2, 4, lo);
	SWAP(8, 10, lo);
	SWAP(9, 11, lo);
	SWAP(0, 7, lo);
	SWAP(5, 12, lo);
	SWAP(2, 3, lo);
	SWAP(9, 10, lo);
	SWAP(1, 8, lo);
	SWAP(4, 11, lo);
	SWAP(6, 12, lo);
	SWAP(2, 9, lo);
	SWAP(1, 7, lo);
	SWAP(3, 10, lo);
	SWAP(6, 11, lo);
	SWAP(2, 8, lo);
	SWAP(4, 10, lo);
	SWAP(2, 7, lo);
	SWAP(5, 10, lo);
	SWAP(4, 8, lo);
	SWAP(6, 10, lo);
	SWAP(3, 7, lo);
	SWAP(5, 9, lo);
	SWAP(4, 7, lo);
	SWAP(6, 9, lo);
	SWAP(5, 7, lo);
	SWAP(6, 8, lo);
	SWAP(6, 7, lo);
}

static __inline__ void sort15(long int * data, long int lo) {
	SWAP(1, 2, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);
	SWAP(7, 8, lo);
	SWAP(9, 10, lo);
	SWAP(11, 12, lo);
	SWAP(13, 14, lo);
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(4, 6, lo);
	SWAP(7, 9, lo);
	SWAP(8, 10, lo);
	SWAP(11, 13, lo);
	SWAP(12, 14, lo);
	SWAP(0, 1, lo);
	SWAP(4, 5, lo);
	SWAP(2, 6, lo);
	SWAP(8, 9, lo);
	SWAP(12, 13, lo);
	SWAP(7, 11, lo);
	SWAP(10, 14, lo);
	SWAP(0, 4, lo);
	SWAP(1, 5, lo);
	SWAP(8, 12, lo);
	SWAP(9, 13, lo);
	SWAP(6, 14, lo);
	SWAP(0, 3, lo);
	SWAP(2, 5, lo);
	SWAP(8, 11, lo);
	SWAP(10, 13, lo);
	SWAP(1, 3, lo);
	SWAP(2, 4, lo);
	SWAP(9, 11, lo);
	SWAP(10, 12, lo);
	SWAP(0, 8, lo);
	SWAP(5, 13, lo);
	SWAP(2, 3, lo);
	SWAP(10, 11, lo);
	SWAP(0, 7, lo);
	SWAP(1, 9, lo);
	SWAP(4, 12, lo);
	SWAP(6, 13, lo);
	SWAP(2, 10, lo);
	SWAP(1, 7, lo);
	SWAP(3, 11, lo);
	SWAP(6, 12, lo);
	SWAP(2, 9, lo);
	SWAP(4, 11, lo);
	SWAP(2, 8, lo);
	SWAP(5, 11, lo);
	SWAP(2, 7, lo);
	SWAP(6, 11, lo);
	SWAP(4, 8, lo);
	SWAP(5, 9, lo);
	SWAP(3, 7, lo);
	SWAP(6, 10, lo);
	SWAP(4, 7, lo);
	SWAP(6, 9, lo);
	SWAP(5, 7, lo);
	SWAP(6, 8, lo);
	SWAP(6, 7, lo);
}

double start_time, end_time, orig_time, write_timer_start, write_timer_end, total_time_write;
void set_time(int timer) {
	gettimeofday(&t, NULL);
	if (timer == 0) {
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 1) {
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 2) {
		orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 3) {
		write_timer_start = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 4) {
		write_timer_end = 1.0e-6 * t.tv_usec + t.tv_sec;
	}
}

void write_long_chunk(int sockfd_client, char *num) {
	set_time(3);
	unsigned int size = sizeof(long int) * SOCKET_BLK;
	int rlen = 0;
	int ret;

	while (rlen < size) {
		if ((ret = write(sockfd_client, (num + rlen), size - rlen)) == -1) {
			fprintf(log_file, "write_long\n");
			fflush(log_file);
			exit(1);
		}

		if (ret <= 0) {
			fprintf(log_file, "socket closed before consumptionn");
			fflush(log_file);
			exit(1);
		}
		rlen += ret;
	}

	set_time(4);
	total_time_write += (write_timer_end - write_timer_start);
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

void merge(long int lo, long int mid, long int hi) {
	long int i, j, k;
	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
//	for (long int i = lo; i <= hi; i++) {
//		temp[i] = data[i];
//	}
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
	long int num = hi - lo + 1;
	if (num <= 15) {
		switch (num) {
		default:
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
		case 8:
			sort8(data, lo);
			return;
		case 9:
			sort9(data, lo);
			return;
		case 10:
			sort10(data, lo);
			return;
		case 11:
			sort11(data, lo);
			return;
		case 12:
			sort12(data, lo);
			return;
		case 13:
			sort13(data, lo);
			return;
		case 14:
			sort14(data, lo);
			return;
		case 15:
			sort15(data, lo);
			return;
		}
	}

	long int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
	if (!(data[mid] < data[mid + 1])) {
//		Merge only if data is sorted
		merge(lo, mid, hi);
	}
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
	log_file = fopen("log", "w+");
	if (argc < 2) {
		printf("Usage ./mem_sort_client IP_server PORT_server\n");
		exit(1);
	}
	set_time(2);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Creating and connecting sockets here*/
	int sockfd_client;
	struct sockaddr_in saddr_client;

	if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(log_file, "Socket failed!\n");
		fflush(log_file);
		exit(1);
	}

	memset(&saddr_client, 0, sizeof(struct sockaddr_in));
	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons((short) atoi(argv[2]));
	saddr_client.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sockfd_client, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1) {
		fprintf(log_file, "Connect failed!\n");
		fflush(log_file);
		exit(1);
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 1 STARTED*/
	char outfile[100];
	FILE *in_file = fopen("temp", "r");
	if (!in_file) {
		printf("Input file missing\n");
		exit(1);
	}

	sprintf(outfile, "temp_lvl%d", 0);
	FILE *out_file = fopen(outfile, "w+");
	if (!out_file) {
		printf("Unable to create output file\n");
		exit(1);
	}

	fseek(in_file, 0, SEEK_END);
	FSIZE = ftell(in_file);
	fseek(in_file, 0, SEEK_SET);
	fclose(in_file);
	SIZE = (FSIZE / 8) / NUM_BLK;
	RSIZE = FSIZE / NUM_BLK;
	fprintf(log_file, "RSIZE = %ld \tBUFSIZ = %d\t NUM_THREADS = %d\t FSIZE = %ld\t SIZE = %ld\t NUM_BLK = %ld \t FITTING %f\n", RSIZE, BUFSIZ, NUM_THREADS, FSIZE, SIZE, NUM_BLK,
			(SIZE * 1.0f) / (NUM_THREADS * 1.0f));
	fflush(log_file);

	data = malloc(sizeof(long int) * SIZE);
	temp = malloc(sizeof(long int) * SIZE);
	if (data == NULL || temp == NULL) {
		perror("Malloc :");
		exit(1);
	}

	for (int blk = 0; blk < NUM_BLK; blk++) {
		fprintf(log_file, "\nStarting with BLK = %d\n", blk);
		fflush(log_file);
		set_time(0);

		FILE *in_file = fopen("temp", "r");
		fseek(in_file, blk * RSIZE, SEEK_SET);
		long int count = 0;
		long int buf_num = BUFSIZ / 8;
		bool flag = false;
		for (long int count = 0; count < SIZE; count++) {
			if (fread(&data[count], sizeof(long int), 1, in_file) == -1) {
				perror("fread");
				exit(1);
			}
		}
		fclose(in_file);

		set_time(1);
		fprintf(log_file, "Reading data completed \t Execution time =  %lf seconds\n", end_time - start_time);
		fflush(log_file);

		set_time(0);
		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}
		set_time(1);

		fprintf(log_file, "Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);
		fflush(log_file);

		set_time(0);
		k_way_single();
		set_time(1);

		fprintf(log_file, "K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);

		fflush(log_file);
		set_time(0);

//		if (is_sorted(0, SIZE - 1) == true) {
//			fprintf(log_file, "Sorted correctly \n");
//		} else {
//			fprintf(log_file, "Sorting error \n");
//			exit(1);
//		}

		set_time(1);
		fprintf(log_file, "Testing completed \t Execution time =  %lf seconds\n", end_time - start_time);
		fflush(log_file);

		set_time(0);
		count = 0;
		buf_num = BUFSIZ / 8;
		flag = false;

		if (blk == 0) {
			for (long int count = 0; count < SIZE; count++) {
				if (fwrite(&data[count], sizeof(long int), 1, out_file) == -1) {
					perror("fwrite");
					exit(1);
				}
			}
			set_time(1);
		} else if (blk == 1) {
			/*Instead of writing the second block , we can directly merge the 1st block using temp*/
		}
		fprintf(log_file, "Writing completed to file %s\t Execution time =  %lf seconds\n", outfile, end_time - start_time);
		fflush(log_file);
	}
	fclose(out_file);

	set_time(1);
	fprintf(log_file, "PHASE 1 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);
	fflush(log_file);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 2 STARTED*/

	int LVL = 0;

	set_time(0);
	char f1[100];
	char remove_fn[100];
	sprintf(f1, "temp_lvl%d", LVL);
	FILE *first_file = fopen(f1, "r");
	NUM_ELE = ELE_PER_PC >> 1;

	long int count1 = 0;
	long int count2 = 0;
	long int pos = 0;
	long int num1;
	long int loc1 = 0;
	fseek(first_file, loc1, SEEK_SET);
	fprintf(log_file, "ST : first_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), NUM_ELE);
	fflush(log_file);
	int ret = -1;
	if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
		printf("fread  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
		exit(1);
	}

	while (true) {
		if (num1 <= data[count2]) {
			if (pos < NUM_ELE) {
				temp[pos] = num1;
			} else {
				data[pos % NUM_ELE] = num1;
			}
			pos++;
			count1++;
			if (count1 >= NUM_ELE)
				break;
			if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
				printf("fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
				exit(1);
			}
		} else {
			if (pos < NUM_ELE) {
				temp[pos] = data[count2];
			} else {
				data[pos % NUM_ELE] = data[count2];
			}
			pos++;
			count2++;

			if (count2 >= NUM_ELE)
				break;
		}
	}

	while (count1 < NUM_ELE) {
		if (pos < NUM_ELE) {
			temp[pos] = num1;
		} else {
			data[pos % NUM_ELE] = num1;
		}
		pos++;
		count1++;
		if (count1 >= NUM_ELE)
			break;
		if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
			printf("while fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
			exit(1);
		}
	}

	while (count2 < NUM_ELE) {
		if (pos < NUM_ELE) {
			temp[pos] = data[count2];
		} else {
			data[pos % NUM_ELE] = data[count2];
		}
		pos++;
		count2++;
		if (count2 >= NUM_ELE)
			break;
	}

	fprintf(log_file, "EN : first_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), NUM_ELE);
	fflush(log_file);
	fclose(first_file);
	set_time(1);

	/*Remove all temporary files*/
	if (remove("temp_lvl0") != 0) {
		perror("Error deleting file");
	}

	fprintf(log_file, "Merge LVL = %d Completed\t Execution time =  %lf seconds \n\n\n", LVL + 1, end_time - start_time);
	fflush(log_file);

	set_time(1);

	fprintf(log_file, "PHASE 2 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
	fflush(log_file);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 3 STARTED*/
//	long int tem_num;
	for (long int i = 0; i < ELE_PER_PC / SOCKET_BLK; i++) {
		if (i % 10 == 0) {
			fprintf(log_file, "Writing %ld \n", i);
			fflush(log_file);
		}

		if (i * SOCKET_BLK < NUM_ELE) {
			write_long_chunk(sockfd_client, (char *) &temp[i * SOCKET_BLK]);
		} else {
			write_long_chunk(sockfd_client, (char *) &data[(i * SOCKET_BLK) % NUM_ELE]);
		}
	}

	fprintf(log_file, "Total time wasted writing = %f\n", total_time_write);
	fprintf(log_file, "Completed correctly!\n");
	fflush(log_file);

	free(temp);
	free(data);

	/*Cleaning the files here*/
//	if (system("rm -f temp answer sort_client sort_client.c log") == -1)
//		perror("System");
	return 0;

}
