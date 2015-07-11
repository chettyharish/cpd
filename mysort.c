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

#define NUM_THREADS 16
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

long int *data;
long int *temp;
int CURR_THREADS;
int SIZE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];

bool is_sorted(int start, int end) {
	int err_ct = 0;
	for (int i = start; i <= end - 1; i++) {
		if (data[i] > data[i + 1]) {
			printf("ERROR : %ld\t%ld\t%d\t%d\n", data[i], data[i + 1], i, i + 1);
			err_ct++;
		}
	}
	if (err_ct > 0) {
		printf("err_ct = %d\n", err_ct);
		return false;
	}
	return true;
}

static __inline__ void sort8(long int * data, int lo) {
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
static __inline__ void sort7(long int * data, int lo) {
	SWAP(1, 2, lo);
	SWAP(0, 2, lo);
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);
	SWAP(3, 5, lo);
	SWAP(4, 6, lo);
	SWAP(4, 5, lo);
	SWAP(0, 4, lo);
	SWAP(0, 3, lo);
	SWAP(1, 5, lo);
	SWAP(2, 6, lo);
	SWAP(2, 5, lo);
	SWAP(1, 3, lo);
	SWAP(2, 4, lo);
	SWAP(2, 3, lo);
}

static __inline__ void sort6(long int * data, int lo) {
	SWAP(1, 2, lo);
	SWAP(0, 2, lo);
	SWAP(0, 1, lo);
	SWAP(4, 5, lo);
	SWAP(3, 5, lo);
	SWAP(3, 4, lo);
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);
	SWAP(2, 5, lo);
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);
	SWAP(2, 3, lo);
}

static __inline__ void sort5(long int * data, int lo) {
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 4, lo);
	SWAP(2, 3, lo);
	SWAP(0, 3, lo);
	SWAP(0, 2, lo);
	SWAP(1, 4, lo);
	SWAP(1, 3, lo);
	SWAP(1, 2, lo);
}

static __inline__ void sort4(long int * data, int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);
	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(1, 2, lo);
}

static __inline__ void sort3(long int * data, int lo) {
	SWAP(1, 2, lo);
	SWAP(0, 2, lo);
	SWAP(0, 1, lo);
}

static __inline__ void sort2(long int * data, int lo) {
	SWAP(0, 1, lo);
}

void merge(int lo, int mid, int hi) {
	int i, j, k;
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

void insertionsort(int lo, int hi) {
	int temp;
	for (int i = lo; i <= hi; i++) {
		for (int j = i; j > lo && data[j] < data[j - 1]; j--) {
			temp = data[j];
			data[j] = data[j - 1];
			data[j - 1] = temp;
		}
	}
}

void mergesort(int lo, int hi) {
	int num = hi - lo + 1;
//	if (hi - lo < 7) {
//		/*Use insertion sort at finer grain level*/
//		insertionsort(lo, hi);
//		return;
//	}

	if (num <= 8) {
		if (num == 0 || num == 1)
			return;
		switch (num) {
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
			sort8(temp, lo);
			return;
		default:
			printf("Negative index in mergesort\n");
			exit(1);
		}
	}

	int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
	if (!(data[mid] < data[mid + 1])) {
//		No need to merge if already sorted
		merge(lo, mid, hi);
	}
}

void *mergesort_caller(void *arg) {
	int myid = *(int *) arg;
	int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
	int start = myid * num_ele;
	int end = (((myid + 1) * num_ele - 1) < (SIZE - 1)) ? ((myid + 1) * num_ele - 1) : (SIZE - 1);
	mergesort(start, end);
}

int find_min(long int *vals) {
	int min = vals[0];
	int pos = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		if (vals[i] < min) {
			min = vals[i];
			pos = i;
		}
	}
	return pos;
}

void *k_way_merger_single(void *arg) {
	int myid = *(int *) arg;
	int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
	int start1 = myid * num_ele;
	int end1 = myid * num_ele + num_ele / 2 - 1;
	int start2 = myid * num_ele + num_ele / 2;
	int end2 = (myid + 1) * num_ele - 1;
	if (end2 >= SIZE) {
		end2 = SIZE - 1;
	}

	int curr1 = start1, curr2 = start2;

//	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);
	int i = start1;
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

	for (int i = start1; i <= end2; i++)
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

	double start_time, end_time;
	struct timeval t;
	int count = 0;
	FILE *file = fopen(argv[1], "r");
	if (!file) {
		printf("Input file missing\n");
		exit(0);
	}

	FILE *file2 = fopen("answer.txt", "w+");
	fseek(file, 0, SEEK_END);
	int FSIZE = ftell(file);
	SIZE = FSIZE / 8;
	data = malloc(sizeof(long int) * SIZE);
	temp = malloc(sizeof(long int) * SIZE);

	if (data == NULL && temp == NULL) {
		perror("Malloc :");
		exit(1);
	}
	fseek(file, 0, SEEK_SET);
	printf("BUFSIZ = %d\t NUM_THREADS = %d\t SIZE = %d\n", BUFSIZ, NUM_THREADS, SIZE);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Reading data started \n");

	int cond = atoi(argv[2]);
	if (cond == 1) {
		while (fread(&data[count++], sizeof(long int), 1, file))
			;
	}

	if (cond == 2) {

		int buf_num = BUFSIZ / 8;
		while (fread(&data[count], sizeof(long int), buf_num, file)) {
			count += buf_num;
		}
	}

	if (cond == 3) {
		int fd = open(argv[1], O_RDONLY);
		long int *map = mmap(0, FSIZE, PROT_READ, MAP_SHARED, fd, 0);
		if (map == MAP_FAILED) {
			perror("mmap : ");
			exit(1);
		}

		for (int i = 0; i < SIZE; i++) {
			data[i] = map[i];
		}
	}

	if (cond == 4) {
		if (fread(data, FSIZE, 1, file) == -1) {
			perror("fread");
			exit(1);
		}
	}

	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Reading data completed \t Execution time =  %lf seconds\n", end_time - start_time);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Split Merge Sort started \n");
	for (int i = 0; i < NUM_THREADS; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}
	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("K-Way Merge started \n");
	k_way_single();
	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
//	printf("Testing started \n");
//	for (int i = 0; i < NUM_THREADS; i++) {
//		int myid = i;
//		int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
//		int start = myid * num_ele;
//		int end = (((myid + 1) * num_ele) < SIZE ? ((myid + 1) * num_ele) : SIZE - 1);
//		if (is_sorted(start, end) == true) {
//			printf("TID = %d \t Sorted correctly\n", i);
//		} else {
//			printf("TID = %d \t Sorting error\n", i);
//		}
//	}

	printf("Testing total array\n");
	if (is_sorted(0, SIZE - 1) == true) {
		printf("Sorted correctly\n");
	} else {
		printf("Sorting error\n");
	}

	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Testing completed \t Execution time =  %lf seconds\n", end_time - start_time);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("\nWriting back\n");
	for (int i = 0; i < SIZE; i++) {
		if (i % 1000 == 0) {
			fprintf(file2, " i = %04d \t num = %ld\n", i, data[i]);
			fflush(file2);
		}
	}

	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Writing completed \t Execution time =  %lf seconds\n", end_time - start_time);
}
