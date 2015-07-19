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

int main(int argc, char **argv) {
	FILE *file1 = fopen(argv[1], "r");
	FILE *file2 = fopen("temp1.data", "w+");
	long int num1;
	long int count = 0;
	fseek(file1, 0, SEEK_END);
	long int SIZE = ftell(file1) / 8;
	fseek(file1, 0, SEEK_SET);

	printf("Starting to read file\n");
	for (long int i = 0; i < SIZE; i++) {
		if (i % 100000000 == 0)
			printf("Writing element %10ld\n", i);
		if (fread(&num1, sizeof(long int), 1, file1) == -1) {
			printf("Error reading\n");
			exit(1);
		}
		fprintf(file2, "%ld\n", num1);
	}
	fflush(file2);
	printf("Starting to sort the data\n");
	if (system("sort -n temp1.data -o temp_sort")) {
		perror("Error sorting");
	}
}
