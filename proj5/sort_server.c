//#include <time.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdbool.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <sys/wait.h>
//#include <sys/time.h>
//#include <sys/mman.h>
//#include <unistd.h>
//#include <math.h>
//#include <pthread.h>
//#include <unistd.h>
//#include <errno.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <limits.h>
//
//#define NAME_LEN 1000
//#define NUM_THREADS 16
//#define MAXCONN 7
//#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }
//
//double start_time, end_time, orig_time;
//struct timeval t;
//long int *data;
//long int *temp;
//const long int ELE_PER_PC = 1000000000;
//const long int NUM_BLK = 2;
//long int CURR_THREADS;
//long int SIZE = 500000000;
//long int RSIZE = 4000000000;
//long int FSIZE;
//int myid[NUM_THREADS];
//pthread_t tid[NUM_THREADS];
//long int consumed[8];
//long int nums[8];
//
//void set_time(int timer) {
//	gettimeofday(&t, NULL);
//	if (timer == 0) {
//		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
//	} else if (timer == 1) {
//		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
//	} else if (timer == 2) {
//		orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;
//	}
//}
//
//static __inline__ void read_long(int sockfd_client, char *num) {
//	unsigned int size = sizeof(long int);
//	int rlen = 0;
//	int ret;
//
//	while (rlen < size) {
//		if ((ret = read(sockfd_client, (num + rlen), size - rlen)) == -1) {
//			perror("read_long");
//			exit(1);
//		}
//
//		if (ret < 0) {
//			perror("socket closed before consumption");
//			exit(1);
//		}
//		rlen += ret;
//	}
//}
//
//static __inline__ int compare_all() {
//	/*Looks at the 8 elements in the array for 8 way socket merge*/
//	long int min = LONG_MAX;
//	int pos = -1;
//	for (int i = 0; i < 8; i++) {
//		if (consumed[i] < ELE_PER_PC) {
//			if (nums[i] < min) {
//				min = nums[i];
//				pos = i;
//			}
//		}
//	}
//
//	return pos;
//}
//
//void tokenize(char *buffer_temp, char *exec_args[]) {
//	int counter = 0;
//	char *token = strtok(buffer_temp, " ");
//	while (token != NULL) {
//		exec_args[counter] = token;
//		counter++;
//		token = strtok(NULL, " ");
//	}
//	exec_args[counter] = NULL;
//}
//
//bool is_sorted(long int start, long int end) {
//	int err_ct = 0;
//	for (long int i = start; i <= end - 1; i++) {
//		if (data[i] > data[i + 1]) {
//			printf("ERROR : %ld\t%ld\t%ld\t%ld\n", data[i], data[i + 1], i, i + 1);
//			err_ct++;
//		}
//	}
//	if (err_ct > 0) {
//		printf("err_ct = %d\n", err_ct);
//		return false;
//	}
//	return true;
//}
//
//static __inline__ void sort2(long int * data, long int lo) {
//	SWAP(0, 1, lo);
//}
//
//static __inline__ void sort3(long int * data, long int lo) {
//	SWAP(1, 2, lo);
//	SWAP(0, 2, lo);
//	SWAP(0, 1, lo);
//}
//
//static __inline__ void sort4(long int * data, long int lo) {
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(0, 2, lo);
//	SWAP(1, 3, lo);
//	SWAP(1, 2, lo);
//}
//static __inline__ void sort5(long int * data, long int lo) {
//
//	SWAP(0, 1, lo);
//	SWAP(3, 4, lo);
//	SWAP(2, 4, lo);
//	SWAP(2, 3, lo);
//	SWAP(1, 4, lo);
//	SWAP(0, 3, lo);
//	SWAP(0, 2, lo);
//	SWAP(1, 3, lo);
//	SWAP(1, 2, lo);
//}
//static __inline__ void sort6(long int * data, long int lo) {
//	SWAP(1, 2, lo);
//	SWAP(4, 5, lo);
//	SWAP(0, 2, lo);
//	SWAP(3, 5, lo);
//	SWAP(0, 1, lo);
//	SWAP(3, 4, lo);
//	SWAP(2, 5, lo);
//	SWAP(0, 3, lo);
//	SWAP(1, 4, lo);
//	SWAP(2, 4, lo);
//	SWAP(1, 3, lo);
//	SWAP(2, 3, lo);
//}
//static __inline__ void sort7(long int * data, long int lo) {
//	SWAP(1, 2, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(0, 2, lo);
//	SWAP(3, 5, lo);
//	SWAP(4, 6, lo);
//	SWAP(0, 1, lo);
//	SWAP(4, 5, lo);
//	SWAP(2, 6, lo);
//	SWAP(0, 4, lo);
//	SWAP(1, 5, lo);
//	SWAP(0, 3, lo);
//	SWAP(2, 5, lo);
//	SWAP(1, 3, lo);
//	SWAP(2, 4, lo);
//	SWAP(2, 3, lo);
//}
//static __inline__ void sort8(long int * data, long int lo) {
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(0, 2, lo);
//	SWAP(1, 3, lo);
//	SWAP(4, 6, lo);
//	SWAP(5, 7, lo);
//	SWAP(1, 2, lo);
//	SWAP(5, 6, lo);
//	SWAP(0, 4, lo);
//	SWAP(3, 7, lo);
//	SWAP(1, 5, lo);
//	SWAP(2, 6, lo);
//	SWAP(1, 4, lo);
//	SWAP(3, 6, lo);
//	SWAP(2, 4, lo);
//	SWAP(3, 5, lo);
//	SWAP(3, 4, lo);
//}
//
//static __inline__ void sort9(long int * data, long int lo) {
//	SWAP(0, 1, lo);
//	SWAP(3, 4, lo);
//	SWAP(6, 7, lo);
//	SWAP(1, 2, lo);
//	SWAP(4, 5, lo);
//	SWAP(7, 8, lo);
//	SWAP(0, 1, lo);
//	SWAP(3, 4, lo);
//	SWAP(6, 7, lo);
//	SWAP(0, 3, lo);
//	SWAP(3, 6, lo);
//	SWAP(0, 3, lo);
//	SWAP(1, 4, lo);
//	SWAP(4, 7, lo);
//	SWAP(1, 4, lo);
//	SWAP(2, 5, lo);
//	SWAP(5, 8, lo);
//	SWAP(2, 5, lo);
//	SWAP(1, 3, lo);
//	SWAP(5, 7, lo);
//	SWAP(2, 6, lo);
//	SWAP(4, 6, lo);
//	SWAP(2, 4, lo);
//	SWAP(2, 3, lo);
//	SWAP(5, 6, lo);
//}
//
//static __inline__ void sort10(long int * data, long int lo) {
//
//	SWAP(4, 9, lo);
//	SWAP(3, 8, lo);
//	SWAP(2, 7, lo);
//	SWAP(1, 6, lo);
//	SWAP(0, 5, lo);
//	SWAP(1, 4, lo);
//	SWAP(6, 9, lo);
//	SWAP(0, 3, lo);
//	SWAP(5, 8, lo);
//	SWAP(0, 2, lo);
//	SWAP(3, 6, lo);
//	SWAP(7, 9, lo);
//	SWAP(0, 1, lo);
//	SWAP(2, 4, lo);
//	SWAP(5, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(1, 2, lo);
//	SWAP(4, 6, lo);
//	SWAP(7, 8, lo);
//	SWAP(3, 5, lo);
//	SWAP(2, 5, lo);
//	SWAP(6, 8, lo);
//	SWAP(1, 3, lo);
//	SWAP(4, 7, lo);
//	SWAP(2, 3, lo);
//	SWAP(6, 7, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(4, 5, lo);
//}
//static __inline__ void sort11(long int * data, long int lo) {
//
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(1, 3, lo);
//	SWAP(5, 7, lo);
//	SWAP(0, 2, lo);
//	SWAP(4, 6, lo);
//	SWAP(8, 10, lo);
//	SWAP(1, 2, lo);
//	SWAP(5, 6, lo);
//	SWAP(9, 10, lo);
//	SWAP(1, 5, lo);
//	SWAP(6, 10, lo);
//	SWAP(5, 9, lo);
//	SWAP(2, 6, lo);
//	SWAP(1, 5, lo);
//	SWAP(6, 10, lo);
//	SWAP(0, 4, lo);
//	SWAP(3, 7, lo);
//	SWAP(4, 8, lo);
//	SWAP(0, 4, lo);
//	SWAP(1, 4, lo);
//	SWAP(7, 10, lo);
//	SWAP(3, 8, lo);
//	SWAP(2, 3, lo);
//	SWAP(8, 9, lo);
//	SWAP(2, 4, lo);
//	SWAP(7, 9, lo);
//	SWAP(3, 5, lo);
//	SWAP(6, 8, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(7, 8, lo);
//}
//
//static __inline__ void sort12(long int * data, long int lo) {
//
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(10, 11, lo);
//	SWAP(1, 3, lo);
//	SWAP(5, 7, lo);
//	SWAP(9, 11, lo);
//	SWAP(0, 2, lo);
//	SWAP(4, 6, lo);
//	SWAP(8, 10, lo);
//	SWAP(1, 2, lo);
//	SWAP(5, 6, lo);
//	SWAP(9, 10, lo);
//	SWAP(1, 5, lo);
//	SWAP(6, 10, lo);
//	SWAP(5, 9, lo);
//	SWAP(2, 6, lo);
//	SWAP(1, 5, lo);
//	SWAP(6, 10, lo);
//	SWAP(0, 4, lo);
//	SWAP(7, 11, lo);
//	SWAP(3, 7, lo);
//	SWAP(4, 8, lo);
//	SWAP(0, 4, lo);
//	SWAP(7, 11, lo);
//	SWAP(1, 4, lo);
//	SWAP(7, 10, lo);
//	SWAP(3, 8, lo);
//	SWAP(2, 3, lo);
//	SWAP(8, 9, lo);
//	SWAP(2, 4, lo);
//	SWAP(7, 9, lo);
//	SWAP(3, 5, lo);
//	SWAP(6, 8, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(7, 8, lo);
//}
//
//static __inline__ void sort13(long int * data, long int lo) {
//
//	SWAP(1, 7, lo);
//	SWAP(9, 11, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 8, lo);
//	SWAP(0, 12, lo);
//	SWAP(2, 6, lo);
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 6, lo);
//	SWAP(8, 11, lo);
//	SWAP(7, 12, lo);
//	SWAP(5, 9, lo);
//	SWAP(0, 2, lo);
//	SWAP(3, 7, lo);
//	SWAP(10, 11, lo);
//	SWAP(1, 4, lo);
//	SWAP(6, 12, lo);
//	SWAP(7, 8, lo);
//	SWAP(11, 12, lo);
//	SWAP(4, 9, lo);
//	SWAP(6, 10, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(8, 9, lo);
//	SWAP(10, 11, lo);
//	SWAP(1, 7, lo);
//	SWAP(2, 6, lo);
//	SWAP(9, 11, lo);
//	SWAP(1, 3, lo);
//	SWAP(4, 7, lo);
//	SWAP(8, 10, lo);
//	SWAP(0, 5, lo);
//	SWAP(2, 5, lo);
//	SWAP(6, 8, lo);
//	SWAP(9, 10, lo);
//	SWAP(1, 2, lo);
//	SWAP(3, 5, lo);
//	SWAP(7, 8, lo);
//	SWAP(4, 6, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//}
//
//static __inline__ void sort14(long int * data, long int lo) {
//
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(10, 11, lo);
//	SWAP(12, 13, lo);
//	SWAP(0, 2, lo);
//	SWAP(4, 6, lo);
//	SWAP(8, 10, lo);
//	SWAP(1, 3, lo);
//	SWAP(5, 7, lo);
//	SWAP(9, 11, lo);
//	SWAP(0, 4, lo);
//	SWAP(8, 12, lo);
//	SWAP(1, 5, lo);
//	SWAP(9, 13, lo);
//	SWAP(2, 6, lo);
//	SWAP(3, 7, lo);
//	SWAP(0, 8, lo);
//	SWAP(1, 9, lo);
//	SWAP(2, 10, lo);
//	SWAP(3, 11, lo);
//	SWAP(4, 12, lo);
//	SWAP(5, 13, lo);
//	SWAP(5, 10, lo);
//	SWAP(6, 9, lo);
//	SWAP(3, 12, lo);
//	SWAP(7, 11, lo);
//	SWAP(1, 2, lo);
//	SWAP(4, 8, lo);
//	SWAP(1, 4, lo);
//	SWAP(7, 13, lo);
//	SWAP(2, 8, lo);
//	SWAP(2, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(9, 10, lo);
//	SWAP(11, 13, lo);
//	SWAP(3, 8, lo);
//	SWAP(7, 12, lo);
//	SWAP(6, 8, lo);
//	SWAP(10, 12, lo);
//	SWAP(3, 5, lo);
//	SWAP(7, 9, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(7, 8, lo);
//	SWAP(9, 10, lo);
//	SWAP(11, 12, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//}
//
//static __inline__ void sort15(long int * data, long int lo) {
//
//	SWAP(0, 1, lo);
//	SWAP(2, 3, lo);
//	SWAP(4, 5, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//	SWAP(10, 11, lo);
//	SWAP(12, 13, lo);
//	SWAP(0, 2, lo);
//	SWAP(4, 6, lo);
//	SWAP(8, 10, lo);
//	SWAP(12, 14, lo);
//	SWAP(1, 3, lo);
//	SWAP(5, 7, lo);
//	SWAP(9, 11, lo);
//	SWAP(0, 4, lo);
//	SWAP(8, 12, lo);
//	SWAP(1, 5, lo);
//	SWAP(9, 13, lo);
//	SWAP(2, 6, lo);
//	SWAP(10, 14, lo);
//	SWAP(3, 7, lo);
//	SWAP(0, 8, lo);
//	SWAP(1, 9, lo);
//	SWAP(2, 10, lo);
//	SWAP(3, 11, lo);
//	SWAP(4, 12, lo);
//	SWAP(5, 13, lo);
//	SWAP(6, 14, lo);
//	SWAP(5, 10, lo);
//	SWAP(6, 9, lo);
//	SWAP(3, 12, lo);
//	SWAP(13, 14, lo);
//	SWAP(7, 11, lo);
//	SWAP(1, 2, lo);
//	SWAP(4, 8, lo);
//	SWAP(1, 4, lo);
//	SWAP(7, 13, lo);
//	SWAP(2, 8, lo);
//	SWAP(11, 14, lo);
//	SWAP(2, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(9, 10, lo);
//	SWAP(11, 13, lo);
//	SWAP(3, 8, lo);
//	SWAP(7, 12, lo);
//	SWAP(6, 8, lo);
//	SWAP(10, 12, lo);
//	SWAP(3, 5, lo);
//	SWAP(7, 9, lo);
//	SWAP(3, 4, lo);
//	SWAP(5, 6, lo);
//	SWAP(7, 8, lo);
//	SWAP(9, 10, lo);
//	SWAP(11, 12, lo);
//	SWAP(6, 7, lo);
//	SWAP(8, 9, lo);
//}
//
//void merge(long int lo, long int mid, long int hi) {
//	long int i, j, k;
////	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
//	for (long int i = lo; i <= hi; i++) {
//		temp[i] = data[i];
//	}
//	i = lo, j = mid + 1;
//
//	for (k = lo; k <= hi; k++) {
//		if (i > mid)
//			data[k] = temp[j++];
//		else if (j > hi)
//			data[k] = temp[i++];
//		else if (temp[i] < temp[j])
//			data[k] = temp[i++];
//		else
//			data[k] = temp[j++];
//	}
//}
//
//void mergesort(long int lo, long int hi) {
//	if (hi <= lo)
//		return;
//	long int num = hi - lo + 1;
//	if (num <= 8) {
//		switch (num) {
//		case 0:
//		case 1:
//			return;
//		case 2:
//			sort2(data, lo);
//			return;
//		case 3:
//			sort3(data, lo);
//			return;
//		case 4:
//			sort4(data, lo);
//			return;
//		case 5:
//			sort5(data, lo);
//			return;
//		case 6:
//			sort6(data, lo);
//			return;
//		case 7:
//			sort7(data, lo);
//			return;
////		case 8:
////			sort8(data, lo);
////			return;
//		}
//	}
//
//	long int mid = lo + (hi - lo) / 2;
//	mergesort(lo, mid);
//	mergesort(mid + 1, hi);
////	if (!(data[mid] < data[mid + 1])) {
////		Merge only if data is sorted
//	merge(lo, mid, hi);
////	}
//}
//
//void *mergesort_caller(void *arg) {
//	int myid = *(int *) arg;
//	long int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
//	long int start = myid * num_ele;
//	long int end = (((myid + 1) * num_ele - 1) < (SIZE - 1)) ? ((myid + 1) * num_ele - 1) : (SIZE - 1);
//	mergesort(start, end);
//}
//
//void *k_way_merger_single(void *arg) {
//	int myid = *(int *) arg;
//	long int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
//	long int start1 = myid * num_ele;
//	long int end1 = myid * num_ele + num_ele / 2 - 1;
//	long int start2 = myid * num_ele + num_ele / 2;
//	long int end2 = (myid + 1) * num_ele - 1;
//	if (end2 >= SIZE) {
//		end2 = SIZE - 1;
//	}
//
//	long int curr1 = start1, curr2 = start2;
//
////	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);
//	long int i = start1;
//	while (curr1 <= end1 && curr2 <= end2) {
//		if (data[curr1] <= data[curr2])
//			temp[i++] = data[curr1++];
//		else
//			temp[i++] = data[curr2++];
//	}
//
//	while (curr1 <= end1) {
//		temp[i++] = data[curr1++];
//	}
//
//	while (curr2 <= end2) {
//		temp[i++] = data[curr2++];
//	}
//
//	for (long int i = start1; i <= end2; i++)
//		data[i] = temp[i];
//}
//
//void k_way_single() {
//	CURR_THREADS = NUM_THREADS >> 1;
//	while (CURR_THREADS != 0) {
//		for (int i = 0; i < CURR_THREADS; i++) {
//			myid[i] = i;
//			pthread_create(&tid[i], NULL, &k_way_merger_single, &myid[i]);
//		}
//
//		for (int i = 0; i < CURR_THREADS; i++) {
//			pthread_join(tid[i], NULL);
//		}
//
//		CURR_THREADS = CURR_THREADS >> 1;
//	}
//
//}
//
//int main(int argc, char **argv) {
//
//	if (argc < 4) {
//		printf("Usage ./sort_server large_file_name current_ip_address machinefile\n");
//		exit(1);
//	}
//	set_time(2);
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*Get the 7 machines from machinefile*/
//	FILE *file = fopen(argv[3], "r");
//	char mac_list[MAXCONN][1000];
//	int num_machines = 0;
//	char buffer_machines[NAME_LEN];
//	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
//		num_machines++;
//	}
//
//	if (num_machines < MAXCONN) {
//		printf("Ensure that the file exists and there are exactly 7 machines which are active with 8GB memory free\n");
//		exit(1);
//	}
//	fseek(file, 0, SEEK_SET);
//	for (int i = 0; i < MAXCONN; i++) {
//		/*Read the machines from machine file*/
//		if (fscanf(file, "%s\n", mac_list[i]) == 0) {
//			printf("Error reading machinefile = %s\n", argv[3]);
//			exit(1);
//		} else {
//			printf("Machine i = %s\n", mac_list[i]);
//		}
//
//	}
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*Creating sockets from here*/
//	int sockfd_server, sockfd_client;
//	struct sockaddr_in saddr_server, saddr_client;
//	int sfd_server[MAXCONN];
//	int sfd_client[MAXCONN];
//	socklen_t len = sizeof(saddr_server);
//
//	for (int i = 0; i < MAXCONN; i++) {
//		saddr_server.sin_addr.s_addr = INADDR_ANY;
//		saddr_server.sin_family = AF_INET;
//		saddr_server.sin_port = htons((short) (5555 + i));
//
//		printf("Creating socket for %d\n", i);
//		if ((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//			perror("socket");
//			exit(1);
//		}
//
//		printf("Binding socket for %d\n", i);
//		if (bind(sockfd_server, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) < 0) {
//			perror("bind");
//			exit(1);
//		}
//
//		printf("Getting name socket for %d\n", i);
//		if (getsockname(sockfd_server, (struct sockaddr *) &saddr_server, &len) < 0) {
//			perror("getsockname");
//			exit(1);
//		}
//		sfd_server[i] = sockfd_server;
//		printf("Listening to socket %d\n", i);
//		if (listen(sfd_server[i], 1) < 0) {
//			perror("listen");
//			exit(1);
//		}
//	}
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*PHASE 1 STARTED*/
//	/*Sending files to all the machines*/
//	char buffer_temp[NAME_LEN];
//	char *exec_args[NAME_LEN];
//
//	for (int i = 0; i < MAXCONN; i++) {
//		if (fork() == 0) {
//			for (int i = 0; i < MAXCONN; i++) {
//				close(sfd_server[i]);
//				close(sfd_client[i]);
//			}
//			close(sockfd_client);
//			close(sockfd_server);
//			long int skip = i * 8000000000l;
//
////			sprintf(buffer_temp, "/bin/dd if=%s bs=32M  iflag=skip_bytes,count_bytes skip=%ld count=8000000000 | ssh %s 'cat > temp'", argv[1], skip, mac_list[i]);
////			printf("%s\n", buffer_temp);
////			if (system(buffer_temp) == -1)
////				perror("System");
//
//			sprintf(buffer_temp, "scp sort_client.c %s:", mac_list[i]);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");
//
//			sprintf(buffer_temp, "ssh %s gcc -o sort_client sort_client.c "
//					"-march=native -Ofast -std=c99 -lm -pedantic "
//					"-pthread -fopenmp -funroll-loops", mac_list[i]);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");
//
//			exit(0);
//		}
//	}
//
//	while (wait(NULL) > 0)
//		;
//
//	for (int i = 0; i < MAXCONN; i++) {
//		if (fork() == 0) {
//			for (int i = 0; i < MAXCONN; i++) {
//				close(sfd_server[i]);
//				close(sfd_client[i]);
//			}
//			close(sockfd_client);
//			close(sockfd_server);
//			sprintf(buffer_temp, "/usr/bin/ssh %s ./sort_client %s %d ", mac_list[i], argv[2], (5555 + i));
//			printf("%s\n", buffer_temp);
//			tokenize(buffer_temp, exec_args);
//			if (execv(exec_args[0], exec_args) == -1) {
//				printf("Command execution error!\n");
//			}
//
//			exit(0);
//		}
//	}
//
//	set_time(1);
//	printf("PHASE 1 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);
//	set_time(2);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*PHASE 2 STARTED*/
//	pid_t pid;
//	int status;
//	if ((pid = fork()) == 0) {
//		for (int i = 0; i < MAXCONN; i++) {
//			close(sfd_server[i]);
//			close(sfd_client[i]);
//		}
//		close(sockfd_client);
//		close(sockfd_server);
//		char outfile[100];
//		FILE *in_file = fopen(argv[1], "r");
//		if (!in_file) {
//			printf("Input file missing\n");
//			exit(1);
//		}
//
//		sprintf(outfile, "temp_lvl%d", 0);
//		FILE *out_file = fopen(outfile, "w+");
//		if (!out_file) {
//			printf("Unable to create output file\n");
//			exit(1);
//		}
//
//		fseek(in_file, 0, SEEK_END);
//		FSIZE = ftell(in_file);
//		fseek(in_file, 0, SEEK_SET);
//		fclose(in_file);
//		printf("RSIZE = %ld \tBUFSIZ = %d\t NUM_THREADS = %d\t FSIZE = %ld\t SIZE = %ld\t NUM_BLK = %ld \t FITTING %f\n", RSIZE, BUFSIZ, NUM_THREADS, FSIZE, SIZE, NUM_BLK,
//				(SIZE * 1.0f) / (NUM_THREADS * 1.0f));
//
//		data = malloc(sizeof(long int) * SIZE);
//		temp = malloc(sizeof(long int) * SIZE);
//		if (data == NULL || temp == NULL) {
//			perror("Malloc :");
//			exit(1);
//		}
//
//		for (int blk = 0; blk < NUM_BLK; blk++) {
//			printf("\nStarting with BLK = %d\n", blk);
//			set_time(0);
//			FILE *in_file = fopen(argv[1], "r");
//			/*Adjusting in large file*/
//			fseek(in_file, (blk + 14) * RSIZE, SEEK_SET);
//			long int count = 0;
//			long int buf_num = BUFSIZ / 8;
//			bool flag = false;
//			for (long int count = 0; count < SIZE; count++) {
//				if (fread(&data[count], sizeof(long int), 1, in_file) == -1) {
//					perror("fread");
//					exit(1);
//				}
//			}
//			fclose(in_file);
//
//			set_time(1);
//			printf("Reading data completed \t Execution time =  %lf seconds\n", end_time - start_time);
//
//			set_time(0);
//			for (int i = 0; i < NUM_THREADS; i++) {
//				myid[i] = i;
//				pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
//			}
//
//			for (int i = 0; i < NUM_THREADS; i++) {
//				pthread_join(tid[i], NULL);
//			}
//			set_time(1);
//			printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);
//
//			set_time(0);
//			k_way_single();
//			set_time(1);
//			printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);
//
//			set_time(0);
//
//			if (is_sorted(0, SIZE - 1) == true) {
//				printf("Sorted correctly \n");
//			} else {
//				printf("Sorting error \n");
//				exit(1);
//			}
//
//			set_time(1);
//			printf("Testing completed \t Execution time =  %lf seconds\n", end_time - start_time);
//
//			set_time(0);
//			count = 0;
//			buf_num = BUFSIZ / 8;
//			flag = false;
//
//			for (long int count = 0; count < SIZE; count++) {
//				if (fwrite(&data[count], sizeof(long int), 1, out_file) == -1) {
//					perror("fwrite");
//					exit(1);
//				}
//			}
//
//			set_time(1);
//			printf("Writing completed to file %s\t Execution time =  %lf seconds\n", outfile, end_time - start_time);
//		}
//		fclose(out_file);
//
//		set_time(1);
//		printf("PHASE 2 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);
//		set_time(2);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/*PHASE 3 STARTED*/
//		int CURR_BLK = NUM_BLK >> 1;
//		int LVL = 0;
//		while (CURR_BLK != 0) {
//			set_time(0);
//			char f1[100];
//			char f2[100];
//			char op[100];
//			char remove_fn[100];
//			sprintf(f1, "temp_lvl%d", LVL);
//			sprintf(f2, "temp_lvl%d", LVL);
//			if (CURR_BLK != 1) {
//				sprintf(op, "temp_lvl%d", LVL + 1);
//			} else {
//				sprintf(op, "answer");
//			}
//			FILE *first_file = fopen(f1, "r");
//			FILE *second_file = fopen(f2, "r");
//			FILE *op_file = fopen(op, "wb+");
//			/*Adjusting for less elements by changing it to >> 6*/
//			long int NUM_ELE = ((long int) (FSIZE >> 6)) / ((long int) NUM_BLK / (1 << LVL));
//
//			for (int i = 0; i < CURR_BLK; i++) {
//				long int count1 = 1;
//				long int count2 = 1;
//				long int num1, num2;
//				long int loc1 = (2 * i) * (long int) NUM_ELE * 8;
//				long int loc2 = (2 * i + 1) * (long int) NUM_ELE * 8;
//				long int loc3 = (2 * i) * (long int) NUM_ELE * 8;
//				fseek(first_file, loc1, SEEK_SET);
//				fseek(second_file, loc2, SEEK_SET);
//				fseek(op_file, loc3, SEEK_SET);
//				printf("ST : first_file = %ld\t second_file = %ld\t out_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(op_file), NUM_ELE);
//
//				int ret = -1;
//				if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
//					printf("fread1 OUT   : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//					exit(1);
//				}
//				if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
//					printf("fread2 OUT   : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//					exit(1);
//				}
//
//				while (true) {
//					if (num1 <= num2) {
//						if ((ret = fwrite(&num1, sizeof(long int), 1, op_file)) != 1) {
//							printf("fwrite1 : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//							exit(1);
//						}
//						count1++;
//						if (count1 > NUM_ELE)
//							break;
//						if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
//							printf("fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//							exit(1);
//						}
//					} else {
//						if ((ret = fwrite(&num2, sizeof(long int), 1, op_file)) != 1) {
//							printf("fwrite2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//							exit(1);
//						}
//						count2++;
//
//						if (count2 > NUM_ELE)
//							break;
//						if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
//							printf("fread2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//							exit(1);
//						}
//					}
//				}
//
//				while (count1 <= NUM_ELE) {
//					if ((ret = fwrite(&num1, sizeof(long int), 1, op_file)) != 1) {
//						printf("while fwrite1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//						exit(1);
//					}
//					count1++;
//					if (count1 > NUM_ELE)
//						break;
//					if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
//						printf("while fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//						exit(1);
//					}
//				}
//
//				while (count2 <= NUM_ELE) {
//					if ((ret = fwrite(&num2, sizeof(long int), 1, op_file)) != 1) {
//						printf("while fwrite2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//						exit(1);
//					}
//					count2++;
//					if (count2 > NUM_ELE)
//						break;
//					if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
//						printf("while fread2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
//						exit(1);
//					}
//				}
//				printf("EN : first_file = %ld\t second_file = %ld\t out_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(op_file), NUM_ELE);
//
//			}
//
//			fclose(first_file);
//			fclose(second_file);
//			fclose(op_file);
//
//			set_time(1);
//
//			printf("Merge LVL = %d Completed\t Execution time =  %lf seconds \n\n\n", LVL + 1, end_time - start_time);
//
//			LVL++;
//			if (LVL != 0) {
//				/*removing the old file out*/
//				sprintf(remove_fn, "rm temp_lvl%d", LVL - 1);
//				printf("Removing file command %s\n", remove_fn);
//				if (system(remove_fn) == -1) {
//					printf("Removing file failed");
//				}
//
//			}
//			CURR_BLK = CURR_BLK >> 1;
//		}
//
//		/*Remove all temporary files*/
//		if (system("rm -f temp_lvl*") == -1) {
//			printf("Removing file failed");
//		}
//
//		set_time(1);
//		printf("PHASE 3 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
//		set_time(2);
//		exit(0);
//	} else {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		/*Accepting the connection here*/
//		for (int i = 0; i < MAXCONN; i++) {
//			if ((sockfd_client = accept(sfd_server[i], (struct sockaddr *) (&saddr_client), &len)) < 0) {
//				perror("accept");
//				exit(1);
//			}
//
//			sfd_client[i] = sockfd_client;
//			printf("client[%d] is remote machine = %s, port = %x, %x.\n", i, inet_ntoa(saddr_client.sin_addr), saddr_client.sin_port, ntohs(saddr_client.sin_port));
//		}
//	}
//
//	printf("Before Wait\n");
//	fflush(stdout);
//	waitpid(pid, NULL, 0);
//	printf("After Wait\n");
//	fflush(stdout);
//
////	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*PHASE 4 STARTED*/
//
//	set_time(0);
//	FILE *last_file = fopen("answer", "r");
//	FILE *final = fopen("final_answer", "w+");
//	printf("Done creating the files\n");
//	fflush(stdout);
//
//	for (long int i = 0; i < 8; i++)
//		consumed[i] = 0;
//	for (int i = 0; i < 7; i++) {
//		printf("Trying to read from %d\n", i);
//		fflush(stdout);
//		read_long(sfd_client[i], (char *) &nums[i]);
//	}
//
//	if (fread(&nums[7], sizeof(long int), 1, last_file) == -1) {
//		perror(" OUT fread");
//		exit(1);
//	}
//
//	printf("Initialization done\n");
//	fflush(stdout);
//	int pos = -1;
//	long int total = 0;
//	for (long int all_count = 0; all_count < 8000000000; all_count++) {
//		pos = compare_all();
//		if (all_count % 10 == 0) {
//			fprintf(final, "%ld\n", nums[pos]);
//		}
//		if (all_count % 500000000 == 0) {
//			set_time(1);
//			total = 0;
//			printf("all_count = %ld   reached at %lf seconds \n", all_count, end_time - start_time);
//			for (int i = 0; i < 8; i++) {
//				printf("pos = %10d    val =%10ld\n", i, consumed[i]);
//				total += (consumed[i]);
//			}
//			printf("Total elements consumed = %ld\n" , total);
//
//		}
//		switch (pos) {
//		case 0:
//		case 1:
//		case 2:
//		case 3:
//		case 4:
//		case 5:
//		case 6:
//			if (consumed[pos] < ELE_PER_PC) {
//				read_long(sfd_client[pos], (char *) &nums[pos]);
//			} else {
//				printf("Pos = %d is done    consumed = %ld\n", pos, consumed[pos]);
//			}
//			break;
//		case 7:
//			if (consumed[pos] < ELE_PER_PC) {
//				if (fread(&nums[7], sizeof(long int), 1, last_file) == -1) {
//					perror(" switch fread");
//					exit(1);
//				}
//			} else {
//				printf("Pos = %d is done     consumed = %ld\n", pos, consumed[pos]);
//			}
//			break;
//		}
//
//
//		consumed[pos]++;
//	}
//
//	set_time(1);
//	printf("PHASE 4 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
//	/*Cleaning the files here*/
//	if (system("rm -f answer") == -1)
//		perror("System");
//
//	return 0;
//
//}
