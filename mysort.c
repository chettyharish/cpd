#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>

#include <sys/mman.h>
#define NUM_THREADS 16
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

void merge(int lo, int mid, int hi) {
	int i, j, k;
	for (i = lo; i <= hi; ++i) {
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
	if (hi - lo <= 8) {
		insertionsort(lo, hi);
		return;
	}
	int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
//	if (!(data[mid] < data[mid + 1])) {
//		No need to merge if already sorted
	merge(lo, mid, hi);
//	}
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
//	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);

}

void k_way_single() {
	CURR_THREADS = NUM_THREADS / 2;
	while (CURR_THREADS != 0) {
		for (int i = 0; i < CURR_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &k_way_merger_single, &myid[i]);
		}

		for (int i = 0; i < CURR_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}

//		printf("Starting tests \n");
//		/*For testing*/
//		for (int i = 0; i < CURR_THREADS; i++) {
//			int myid = i;
//			int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
//			int start1 = myid * num_ele;
//			int end1 = myid * num_ele + num_ele / 2 - 1;
//			int start2 = myid * num_ele + num_ele / 2;
//			int end2 = (myid + 1) * num_ele - 1;
//			if (end2 >= SIZE) {
//				end2 = SIZE - 1;
//			}
//			if (is_sorted(start1, end2) == true) {
//				printf("TID = %d \t Sorted correctly\n", i);
//			} else {
//				printf("TID = %d \t Sorting error\n", i);
//			}
//		}

		CURR_THREADS /= 2;
	}

}

int main(int argc, char **argv) {

	double start_time, end_time;
	struct timeval t;
	int count = 0;
	long int waste;
	FILE *file = fopen(argv[1], "r");
	printf("BUFSIZE = %d\n", BUFSIZ);
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

	if (data && temp) {
	} else {
		perro("Malloc");
	}

	fseek(file, 0, SEEK_SET);

	gettimeofday(&t, NULL);
	start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Reading data started \n");
	while (fread(&data[count++], sizeof(long int), 1, file))
		;
//	int buf_num = BUFSIZ/8;
//	while(fread(&data[count], sizeof(long int), buf_num, file))
//	{
//		count += buf_num;
//	}
//	printf("SIZE = %d\t FSIZE = %d\t buf = %d", SIZE, FSIZE, buf_num);
//	printf("count = %d\n" , count);
//	int fd = open(argv[1], O_RDONLY);
//	int pageoffset = FSIZE % 4096;
//	long int *map = (long int *) mmap(0, FSIZE, PROT_READ, MAP_PRIVATE, fd, 0);
//
//	for (int i = 0; i < SIZE; i++) {
//		data[i] = map[i];
//	}

//	if (fread(data, FSIZE, 1, file) == -1) {
//		perror("fread");
//	}

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
