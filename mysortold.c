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

int main(int argc, char **argv) {
	int count = 0;
	long int waste;
	FILE *file = fopen(argv[1], "r");
	FILE *temp3 = fopen("temp3", "w+");
	fseek(file, 0, SEEK_END);
	SIZE = ftell(file) / 8;
	printf("size = %d\t count = %d\n", SIZE, count);
	data = malloc(sizeof(long int) * SIZE);
	temp = malloc(sizeof(long int) * SIZE);

	fseek(file, 0, SEEK_SET);
	printf("size = %d\t count = %d\n", SIZE, count);
	while (fread(&data[count++], sizeof(long int), 1, file))
		;


	mergesort(0 , SIZE - 1);

	printf("size = %d\t count = %d\n", SIZE, count);

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

	printf("size = %d\t count = %d\n", SIZE, count);
	printf("Writing back\n");

	for (int i = 0; i < SIZE; i++) {
		fwrite(&data[i], sizeof(long int), 1, temp3);
	}
	fflush(temp3);
}
