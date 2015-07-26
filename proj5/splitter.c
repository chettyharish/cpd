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

int main(int argc, char **argv) {
	long int num = atol(argv[1]);
	long int num1[100000];
	FILE *file = fopen("../cop5570a/test0", "r");
	FILE *file2 = fopen("op", "w+");
	for (long int i = 0; i < num / 100000; i++) {
		if (i % 100 == 0)
			printf("Working on i = %ld\n", i*100000);
		if (fread(&num1[0], sizeof(long int), 100000, file) == -1)
			perror("fread");
		if (fwrite(&num1[0], sizeof(long int), 100000, file2) == -1)
			perror("fwrite");
	}
}
