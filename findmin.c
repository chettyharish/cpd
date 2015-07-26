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
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

#define MINSWAP(p1,p2) if(nums[pos[p2]] < nums[pos[p1]]){ long int tmp = pos[p1]; pos[p1] = pos[p2]; pos[p2] = tmp;}
int findmin(long int *nums) {
	for (int i = 0; i < 9; i++) {
		printf("%ld\t", nums[i]);
	}
	printf("\n\n");
	int pos[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	MINSWAP(0, 1);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(2, 3);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(4, 5);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(6, 7);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(0, 2);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(4, 6);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(0, 4);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	MINSWAP(0, 8);
	for (int i = 0; i < 9; i++) {
		printf("%d\t", pos[i]);
	}
	printf("\n\n");
	return pos[0];
}

int main(int argc, char **argv) {
	long int nums[9] = { 12, 432, 52, -12, 543, -56, 24, 123, 9 };
	printf("Min Pos = %d\n", findmin(nums));
}
