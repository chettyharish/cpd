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
#include <limits.h>
#include <fcntl.h>

#define SOCKET_BLK 100
#define SEND_SOCKET_BLK 50000000
//#define START_SOCK 23000
int START_SOCK;
#define NAME_LEN 1000
#define NUM_THREADS 16
#define ELE_PER_CLIENT 850000000l
#define ELE_PER_SERVER 1200000000l
#define ELE_PER_BLK 600000000l
#define MAXCONN 8
#define TOTAL_PROCS 9

struct timeval t;
long int *data;
long int *data2;
long int *temp;
long int CURR_THREADS;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];
long int consumed[TOTAL_PROCS];


#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }
//#define likely(x) __builtin_expect((x),1)
//#define unlikely(x) __builtin_expect((x),0)
#define likely(x) (x)
#define unlikely(x) (x)

double start_time = 0, end_time = 0, orig_time = 0, read_timer_start = 0, read_timer_end = 0, compare_start = 0, compare_end = 0;
double total_time_read = 0, total_time_min = 0;
static __inline__ void set_time(int timer) {
	gettimeofday(&t, NULL);
	if (likely(timer == 0)) {
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (likely(timer == 1)) {
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (unlikely(timer == 2)) {
		orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (likely(timer == 3)) {
		read_timer_start = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (likely(timer == 4)) {
		read_timer_end = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (likely(timer == 5)) {
		compare_start = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (likely(timer == 6)) {
		compare_end = 1.0e-6 * t.tv_usec + t.tv_sec;
	}
}


#define printf(...)
#define set_time(...)

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


static __inline__ void read_long_chunk(int sockfd_client, char *num) {
	set_time(3);
	long  int size = sizeof(long int) * SOCKET_BLK;
	long int rlen = 0, ret = 0;
	while (rlen < size) {
		if (unlikely((ret = read(sockfd_client, (num + rlen), size - rlen)) == -1)) {
			perror("read_long");
			exit(1);
		}

		if (unlikely(ret < 0)) {
			perror("socket closed before consumption");
			exit(1);
		}
		rlen += ret;
	}
	set_time(4);
	total_time_read += (read_timer_end - read_timer_start);
}

void write_long_chunk(int sockfd_client, char *num) {
	long int size = sizeof(long int) * SEND_SOCKET_BLK;
	long int rlen = 0, ret = 0;
	while (rlen < size) {
		if (unlikely((ret = write(sockfd_client, (num + rlen), size - rlen)) == -1)) {
			perror("Write long");
			exit(1);
		}
		if (unlikely(ret <= 0)) {
			exit(1);
		}
		rlen += ret;
	}
}

static __inline__ int compare_all(long int *nums) {
	/*Looks at the 9 elements in the array for 9 way socket merge*/
	set_time(5);
	long int min = LONG_MAX;
	int pos = -1;
	for (int i = 0; i < TOTAL_PROCS; i++) {
		if (nums[i] < min) {
			min = nums[i];
			pos = i;
		}
	}
	set_time(6);
	total_time_min += (compare_end - compare_start);
	return pos;
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

void merge(long int lo, long int mid, long int hi) {
	long int i, j, k;
	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
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
	if (likely(!(data[mid] < data[mid + 1]))) {
		/*Merge only if data is sorted*/
		merge(lo, mid, hi);
	}
}

void *mergesort_caller(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((ELE_PER_SERVER * 1.0f) / NUM_THREADS);
	long int start = myid * num_ele;
	long int end = (((myid + 1) * num_ele - 1) < (ELE_PER_BLK - 1)) ? ((myid + 1) * num_ele - 1) : (ELE_PER_BLK - 1);
	mergesort(start, end);
}

void sort_blk() {
	for (int i = 0; i < NUM_THREADS; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}
}

void *k_way_merger_single(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((ELE_PER_BLK * 1.0f) / CURR_THREADS);
	long int start1 = myid * num_ele;
	long int end1 = myid * num_ele + num_ele / 2 - 1;
	long int start2 = myid * num_ele + num_ele / 2;
	long int end2 = (myid + 1) * num_ele - 1;
	if (end2 >= ELE_PER_BLK) {
		end2 = ELE_PER_BLK - 1;
	}

//	printf("myid = %d\tnum_ele = %ld \t start1 = %ld\t end1 = %ld\t start2 = %ld\t end2 = %ld\t ", myid, num_ele, start1, end1, start2, end2);

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
	if (argc < 5) {
		printf("Usage ./server_king large_file_name current_ip_address machine_file start_socket\n");
		printf("large_file_name : the full path with filename for the file with 8B elements\n");
		printf("current_ip_address: The IP address of the computer where you are running the code\n");
		printf("machine_file: List of 8 machines which are authenticated for direct ssh\n");
		printf("start_socket: Uses sockets from the start_socket to start_socket+8 for the 9 machines\n");
		fflush(0);
		exit(1);
	}
	set_time(2);
	START_SOCK = atoi(argv[4]);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Get the 8 machines from machinefile*/
	FILE *file = fopen(argv[3], "r");
	char mac_list[MAXCONN][NAME_LEN];
	int num_machines = 0;
	char buffer_machines[NAME_LEN];
	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
		num_machines++;
	}

	if (num_machines < MAXCONN) {
		printf("Ensure that the file exists and there are exactly 8 machines which are active with 10GB memory free\n");
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
	/*PHASE 1 STARTED*/
	/*Creating sockets from here*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in saddr_server, saddr_client;
	int sfd_server[MAXCONN];
	int sfd_client[MAXCONN];
	socklen_t len = sizeof(saddr_server);

	for (int i = 0; i < MAXCONN; i++) {
		saddr_server.sin_addr.s_addr = INADDR_ANY;
		saddr_server.sin_family = AF_INET;
		saddr_server.sin_port = htons((short) (START_SOCK + i));

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
	/*Sending client executables to all the machines*/
	char buffer_temp[NAME_LEN];
	char *exec_args[NAME_LEN];

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
			sprintf(buffer_temp, "scp client_king.c %s:", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");
			sprintf(buffer_temp, "ssh %s gcc -o client_king client_king.c "
					"-march=native -Ofast -std=c99 -lm -pedantic "
					"-pthread -fopenmp -funroll-loops", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			sprintf(buffer_temp, "/usr/bin/ssh %s ./client_king %s %d ", mac_list[i], argv[2], (START_SOCK + i));
			printf("%s\n", buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
				exit(1);
			}

			exit(0);
		}
	}

	/*Accepting the connection here*/
	for (int i = 0; i < MAXCONN; i++) {
		if ((sockfd_client = accept(sfd_server[i], (struct sockaddr *) (&saddr_client), &len)) < 0) {
			perror("accept");
			exit(1);
		}

		sfd_client[i] = sockfd_client;
		printf("client[%d] is remote machine = %s, port = %d.\n", i, inet_ntoa(saddr_client.sin_addr), ntohs(saddr_client.sin_port));
	}

	fflush(stdout);

	/*Sending data to the receivers*/
	long int *op_data = malloc(sizeof(long) * SEND_SOCKET_BLK);
	int pid[8];
	for (int i = 0; i < MAXCONN; i++) {
		if ((pid[i] = fork()) == 0) {
			long int start = time(0);
			FILE *in_file = fopen(argv[1], "r");
			if (!in_file) {
				printf("Input file missing\n");
				exit(1);
			}
			long int skip = i * ELE_PER_CLIENT * 8l;
			fseek(in_file, skip, SEEK_SET);
			bool f1 = false, f2 = false, f3 = false, f4 = false;
			for (long int total = 0; total < ELE_PER_CLIENT; total += SEND_SOCKET_BLK) {
				if (unlikely(total >= ELE_PER_CLIENT && f1 ==false)) {
					printf("Done transmitting 100 percent in process %d  time = %ld\n", i, time(0) - start);
					f1 = true;
				} else if (unlikely(total >= ELE_PER_CLIENT * 0.75&& f2 ==false)) {
					printf("Done transmitting 75 percent  in process %d  time = %ld\n", i, time(0) - start);
					f2 = true;
				} else if (unlikely(total >= ELE_PER_CLIENT * 0.50&& f3 ==false)) {
					printf("Done transmitting 50 percent  in process %d  time = %ld\n", i, time(0) - start);
					f3 = true;
				} else if (unlikely(total >= ELE_PER_CLIENT * 0.25&& f4 ==false)) {
					printf("Done transmitting 25 percent  in process %d  time = %ld\n", i, time(0) - start);
					f4 = true;
				}

				if (unlikely(fread(op_data, sizeof(long int), SEND_SOCKET_BLK, in_file) == -1)) {
					perror("fread");
					exit(1);
				}
				write_long_chunk(sfd_client[i], (char *) op_data);
			}

			printf("Process %d  Done sending at %ld\n", i, time(0) - start);
			exit(0);
		}
	}

	free(op_data);
	for (int i = 0; i < MAXCONN; i++) {
		waitpid(pid[i], NULL, 0);
	}

	set_time(1);
	printf("PHASE 1 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);
	set_time(2);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 2 STARTED*/
	FILE *in_file = fopen(argv[1], "r");
	if (!in_file) {
		printf("Input file missing\n");
		exit(1);
	}

	data = malloc(sizeof(long int) * ELE_PER_BLK);
	temp = malloc(sizeof(long int) * ELE_PER_BLK);
	data2 = malloc(sizeof(long int) * ELE_PER_BLK);
	if (data == NULL || data2 == NULL || temp == NULL) {
		perror("Malloc :");
		exit(1);
	}

	printf("\nStarting with BLK = 0\n");
	set_time(0);
	fseek(in_file, ELE_PER_CLIENT * sizeof(long int) * MAXCONN, SEEK_SET);
	for (long int count = 0; count < ELE_PER_BLK; count += SEND_SOCKET_BLK ) {
		if (unlikely(fread(&data[count], sizeof(long int), SEND_SOCKET_BLK, in_file) == -1)) {
			perror("fread");
			exit(1);
		}
	}
//	for (long int count = 0; count < ELE_PER_BLK; count++) {
//		if (unlikely(fread(&data[count], sizeof(long int), 1, in_file) == -1)) {
//			perror("fread");
//			exit(1);
//		}
//	}
	set_time(1);
	printf("Reading blk = 0 completed \t Execution time =  %lf seconds\n", end_time - start_time);

	set_time(0);
	sort_blk();
	set_time(1);
	printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);

	set_time(0);
	k_way_single();
	set_time(1);
	printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);

	set_time(0);
	/*Pointer trickery :P*/
	long int *ptr_trick = data;
	data = data2;
	data2 = ptr_trick;
	set_time(1);
	printf("Copying data completed\t Execution time =  %lf seconds\n", end_time - start_time);

	/*Blk 2 processing*/
	printf("\nStarting with BLK = 1\n");
	set_time(0);
	fseek(in_file, (ELE_PER_CLIENT * 1l * MAXCONN + ELE_PER_BLK * 1l) * sizeof(long int), SEEK_SET);
	for (long int count = 0; count < ELE_PER_BLK; count += SEND_SOCKET_BLK ) {
		if (unlikely(fread(&data[count], sizeof(long int), SEND_SOCKET_BLK, in_file) == -1)) {
			perror("fread");
			exit(1);
		}
	}
//	for (long int count = 0; count < ELE_PER_BLK; count++) {
//		if (unlikely(fread(&data[count], sizeof(long int), 1, in_file) == -1)) {
//			perror("fread");
//			exit(1);
//		}
//	}
	set_time(1);
	printf("Reading blk = 1 completed \t Execution time =  %lf seconds\n", end_time - start_time);

	set_time(0);
	sort_blk();
	set_time(1);
	printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);

	set_time(0);
	k_way_single();
	set_time(1);
	printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);
	printf("PHASE 2 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 3 STARTED*/
	set_time(0);
	long int count1 = 0;
	long int count2 = 0;
	long int pos = 0;

	while (count1 < ELE_PER_BLK && count2 < ELE_PER_BLK) {
		if (data2[count1] <= data[count2]) {
			if (pos < ELE_PER_BLK) {
				temp[pos] = data2[count1];
			} else {
				data[pos % ELE_PER_BLK] = data2[count1];
			}
			pos++;
			count1++;
		} else {
			if (pos < ELE_PER_BLK) {
				temp[pos] = data[count2];
			} else {
				data[pos % ELE_PER_BLK] = data[count2];
			}
			pos++;
			count2++;
		}
	}

	while (count1 < ELE_PER_BLK) {
		if (pos < ELE_PER_BLK) {
			temp[pos] = data2[count1];
		} else {
			data[pos % ELE_PER_BLK] = data2[count1];
		}
		pos++;
		count1++;
	}

	while (count2 < ELE_PER_BLK) {
		if (pos < ELE_PER_BLK) {
			temp[pos] = data[count2];
		} else {
			data[pos % ELE_PER_BLK] = data[count2];
		}
		pos++;
		count2++;
	}

	set_time(1);
	printf("PHASE 3 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 4 STARTED*/

	long int nums[TOTAL_PROCS];
	static long int data_chunks[MAXCONN][SOCKET_BLK];
//	long int **data_chunks = (long int **) malloc(MAXCONN * sizeof(long int *));
//	for (int i = 0; i < MAXCONN; i++)
//		data_chunks[i] = (long int *) malloc(SOCKET_BLK * sizeof(long int));
	set_time(0);
	FILE *final = fopen("final_answer", "w+");
	printf("Waiting for all machines\n");
	fflush(stdout);

	for (long int i = 0; i < TOTAL_PROCS; i++) {
		consumed[i] = 0;
	}
	for (int i = 0; i < MAXCONN; i++) {
		read_long_chunk(sfd_client[i], (char *) data_chunks[i]);
		nums[i] = data_chunks[i][0];
		printf("Machine %d has started transmitting \n", i);
	}

	nums[8] = temp[0];
	printf("Initialization done\n");
	fflush(stdout);
	int loc = -1;
	long int total = 0;
	for (long int all_count = 0; all_count < 8000000000; all_count++) {
		loc = compare_all(nums);
		if (all_count % 10 == 0) {
			fprintf(final, "%ld\n", nums[loc]);
		}

		if (unlikely(all_count % 1000000000 == 0)) {
			set_time(1);
			total = 0;
			printf("all_count = %ld   reached at %lf seconds \n", all_count, end_time - start_time);
			for (int i = 0; i < TOTAL_PROCS; i++) {
				printf("pos = %10d    val =%10ld\n", i, consumed[i]);
				total += (consumed[i]);
			}
			printf("Total elements consumed = %ld\n", total);
			fflush(stdout);
		}
		consumed[loc]++;

		switch (loc) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			if (consumed[loc] < ELE_PER_CLIENT) {
				if (consumed[loc] % SOCKET_BLK == 0) {
					/*Getting the next chunk*/
					read_long_chunk(sfd_client[loc], (char *) data_chunks[loc]);
				}
				nums[loc] = data_chunks[loc][consumed[loc] % SOCKET_BLK];
			} else {
//				totally_consumed[loc] = true;
				nums[loc] = LONG_MAX;
				printf("Pos = %d is done    consumed = %ld\n", loc, consumed[loc]);
			}
			break;
		case 8:
			if (consumed[loc] < ELE_PER_SERVER) {
				if (consumed[loc] < ELE_PER_BLK) {
					nums[loc] = temp[(consumed[loc])];
				} else {
					nums[loc] = data[(consumed[loc]) % ELE_PER_BLK];
				}
			} else {
				nums[loc] = LONG_MAX;
				printf("Pos = %d is done     consumed = %ld\n", loc, consumed[loc]);
			}
			break;
		}
	}

	set_time(1);
	printf("PHASE 4 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
	printf("Total time wasted reading = %f\n", total_time_read);
	printf("Total time wasted min = %f\n", total_time_min);

	for(int i = 0 ; i < MAXCONN ; i++){
		printf("Closing connection : %d\n" , i);
		close(sfd_client[i]);
	}
	return 0;

}

