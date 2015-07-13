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
	int count = 0;
	fseek(file1, 0, SEEK_END);
	int SIZE = ftell(file1) / 8;
	long int *data = malloc(sizeof(long int) * SIZE);
	fseek(file1, 0, SEEK_SET);

	while (fread(&data[count++], sizeof(long int), 1, file1))
		;

	for (int i = 0; i < SIZE; i++) {
		if (i % 1000 == 0)
			fprintf(file2, "%10d\t%20ld\n", i, data[i]);
	}
	fflush(file2);
//	system("sort -n temp1.data > temp_sort");
}
