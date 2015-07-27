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
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)


int main(int argc, char **argv) {
	int ip;
	scanf("%d", &ip);
	if (likely(ip < 10)) {
		printf("L 10\n");
	} else {
		printf("G 10\n");
	}
}
