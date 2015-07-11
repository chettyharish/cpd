#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>

long int *data;
long int *temp;
#define NUM_THREADS 32
int CURR_THREADS;
int SIZE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];

bool is_sorted(int start, int end) {
	int err_ct = 0;
	for (int i = start; i < end - 1; i++) {
		if (data[i] > data[i + 1])
			err_ct++;
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

void mergesort(int lo, int hi) {
	if (hi <= lo)
		return;
	int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
	merge(lo, mid, hi);
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
	int i = start1;
	while (curr1 != end1 && curr2 != end2) {
		if (data[curr1] <= data[curr2])
			temp[i++] = data[curr1++];
		else
			temp[i++] = data[curr2++];
	}

	while (curr1 != end1) {
		temp[i++] = data[curr1++];
	}

	while (curr2 != end2) {
		temp[i++] = data[curr2++];
	}

	for (int i = start1; i < end2; i++)
		data[i] = temp[i];

}

void k_way_single() {
	CURR_THREADS = NUM_THREADS;
	while (CURR_THREADS != 1) {
		CURR_THREADS /= 2;
		for (int i = 0; i < CURR_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &k_way_merger_single, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}
	}

}

int main(int argc, char **argv) {
	int count = 0;
	long int waste;
	FILE *file = fopen(argv[1], "r");
	FILE *file2 = fopen("answer.txt", "w+");
	fseek(file, 0, SEEK_END);
	SIZE = ftell(file) / 8;
	printf("size = %d\t count = %d\n", SIZE, count);
	data = malloc(sizeof(long int) * SIZE);
	temp = malloc(sizeof(long int) * SIZE);

	fseek(file, 0, SEEK_SET);
	printf("size = %d\t count = %d\n", SIZE, count);
	while (fread(&data[count++], sizeof(long int), 1, file))
		;

	for (int i = 0; i < NUM_THREADS; i++) {
		myid[i] = i;
		pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(tid[i], NULL);
	}

	k_way_single();

	printf("Starting tests \n");
	/*For testing*/
	for (int i = 0; i < NUM_THREADS; i++) {
		int myid = i;
		int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
		int start = myid * num_ele;
		int end = (((myid + 1) * num_ele) < SIZE ? ((myid + 1) * num_ele) : SIZE);
		if (is_sorted(start, end) == true) {
			printf("TID = %d \t Sorted correctly\n", i);
		} else {
			printf("TID = %d \t Sorting error\n", i);
		}
	}

	printf("Writing back\n");
	for (int i = 0; i < SIZE; i++) {
		if (i % 1000 == 0) {
			fprintf(file2, " i = %04d \t num = %ld\n", i, data[i]);
			fflush(file2);
		}
	}
}
