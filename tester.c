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

int main() {
	/*For testing the final correctness*/
	char outfile[100];
	FILE *t1 = fopen("test0", "r");
	FILE *t2 = fopen("temp_lvl0", "r");
	FILE *t3 = fopen("temp_lvl1", "r");
	FILE *t4 = fopen("temp_lvl2", "r");
	FILE *t5 = fopen("temp_lvl3", "r");
	FILE *t6 = fopen("temp", "r");
	long int n1 = 0;
	long int n2 = 0;
	long int n3 = 0;
	long int n4 = 0;
	long int n5 = 0;
	long int n6 = 0;
	long long int total1 = 0;
	long long int total2 = 0;
	long long int total3 = 0;
	long long int total4 = 0;
	long long int total5 = 0;
	long long int total6 = 0;

	for (int i = 0; i < 80000000 / 8; i++) {
		if (fread(&n1, sizeof(long int), 1, t1) == 0) {
			break;
		}

		if (fread(&n2, sizeof(long int), 1, t2) == 0) {
			break;
		}

		if (fread(&n3, sizeof(long int), 1, t3) == 0) {
			break;
		}

		if (fread(&n4, sizeof(long int), 1, t4) == 0) {
			break;
		}

		if (fread(&n5, sizeof(long int), 1, t5) == 0) {
			break;
		}
		if (fread(&n6, sizeof(long int), 1, t6) == 0) {
			break;
		}
		total1 += n1;
		total2 += n2;
		total3 += n3;
		total4 += n4;
		total5 += n5;
		total6 += n6;
	}
	printf("orig:%lld\ttemp:%lld\tl0:%lld\tl1:%lld\tl2:%lld\t\tl3:%lld\t\n", total1,total6, total2, total3, total4, total5);
}
