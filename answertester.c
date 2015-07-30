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

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage ./a.out f1 f2\n");
		fflush(0);
		exit(1);
	}

	FILE *f1 = fopen(argv[1], "r");
	FILE *f2 = fopen(argv[2], "r");

	long int num1;
	long int num2;
	for (long int i = 0; i < 800000000; i++) {
		int ret = fscanf(f1, "%ld\n", &num1);
		int ret2 = fscanf(f2, "%ld\n", &num2);
		if (num1 != num2) {
			printf("Error due to %ld and %ld at location = %ld\n", num1, num2 , i);
			exit(1);
		}
//
		if (i % 50000000 == 0) {
			printf("Working at %ld\n", i);
		}

	}
}
