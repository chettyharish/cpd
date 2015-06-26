/*
 * Created 06/20/2015 by Xin Yuan for the COP5570 class
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAX_N 8192
#define NUM_THREADS 100

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];

int w_X, w_Y;

void init1(int X, int Y) {
	int i, j;
	w_X = X, w_Y = Y;
	for (i = 0; i < w_X; i++)
		for (j = 0; j < w_Y; j++)
			w[j][i] = 0;

	for (i = 0; i < w_X; i++)
		w[0][i] = 1;
	for (i = 0; i < w_Y; i++)
		w[i][0] = 1;
}

void print_world() {
	int i, j;

	for (i = 0; i < w_Y; i++) {
		for (j = 0; j < w_X; j++) {
			printf("%d", (int) w[i][j]);
		}
		printf("\n");
	}
}

int neighborcount(int x, int y) {
	int count = 0;

	if ((x < 0) || (x >= w_X)) {
		printf("Xneighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}
	if ((y < 0) || (y >= w_Y)) {
		printf("Yneighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}

	if (x == 0) {
		if (y == 0) {
			count = w[y][x + 1] + w[y + 1][x] + w[y + 1][x + 1];
		} else if (y == w_Y - 1) {
			count = w[y][x + 1] + w[y - 1][x] + w[y - 1][x + 1];
		} else {
			count = w[y - 1][x] + w[y + 1][x] + w[y - 1][x + 1] + w[y][x + 1] + w[y + 1][x + 1];
		}
	} else if (x == w_X - 1) {
		if (y == 0) {
			count = w[y][x - 1] + w[y + 1][x - 1] + w[y + 1][x];
		} else if (y == w_Y - 1) {
			count = w[y][x - 1] + w[y - 1][x] + w[y - 1][x - 1];
		} else {
			count = w[y - 1][x] + w[y + 1][x] + w[y - 1][x - 1] + w[y][x - 1] + w[y + 1][x - 1];
		}
	} else { /* x is in the middle */
		if (y == 0) {
			count = w[y][x - 1] + w[y][x + 1] + w[y + 1][x - 1] + w[y + 1][x] + w[y + 1][x + 1];
		} else if (y == w_Y - 1) {
			count = w[y][x - 1] + w[y][x + 1] + w[y - 1][x - 1] + w[y - 1][x] + w[y - 1][x + 1];
		} else {
			count = w[y - 1][x - 1] + w[y][x - 1] + w[y + 1][x - 1] + w[y - 1][x] + w[y + 1][x] + w[y - 1][x + 1] + w[y][x + 1] + w[y + 1][x + 1];
		}
	}

	return count;
}

void *new_world_generator(void *arg) {
	int myid = *(int *) arg;
	int x, y, c;
	int rows_per_blk = ceil((w_Y * 1.0f) / NUM_THREADS);
	int start = myid * rows_per_blk;
	for (x = 0; x < w_X; x++) {
		for (y = 0; y < rows_per_blk; y++) {
			if (start + y >= w_Y)
				break;
			c = neighborcount(x, start + y); /* count neighbors */
			if (c <= 1)
				neww[start + y][x] = 0; /* die of loneliness */
			else if (c >= 4)
				neww[start + y][x] = 0; /* die of overpopulation */
			else if (c == 3)
				neww[start + y][x] = 1; /* becomes alive */
			else
				neww[start + y][x] = w[start + y][x]; /* c == 2, no change */
		}
	}
}

int main(int argc, char *argv[]) {
	int x, y;
	int iter = 0;
	int c;
	int init_count;
	int count;

	if (argc == 1) {
		printf("Usage: ./a.out w_X w_Y\n");
		exit(0);
	}
	/* more than three parameters */
	init1(atoi(argv[1]), atoi(argv[2]));

	c = 0;
	for (x = 0; x < w_X; x++) {
		for (y = 0; y < w_Y; y++) {
			if (w[y][x] == 1)
				c++;
		}
	}

	init_count = c;
	count = init_count;

	printf("initial world, population count: %d\n", c);
	if (DEBUG_LEVEL > 10)
		print_world();

	int myid[NUM_THREADS];
	pthread_t tid[NUM_THREADS];

	for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {

		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &new_world_generator, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}
		/* copy the world, and count the current lives */
		count = 0;
		for (x = 0; x < w_X; x++) {
			for (y = 0; y < w_Y; y++) {
				w[y][x] = neww[y][x];
				if (w[y][x] == 1)
					count++;
			}
		}
		printf("iter = %d, population count = %d\n", iter, count);
		if (DEBUG_LEVEL > 10)
			print_world();
	}

	{
		FILE *fd;
		if ((fd = fopen("final_world000.txt", "w")) != NULL) {
			for (x = 0; x < w_X; x++) {
				for (y = 0; y < w_Y; y++) {
					fprintf(fd, "%d", (int) w[y][x]);
				}
				fprintf(fd, "\n");
			}
		} else {
			printf("Can't open file final_world000.txt\n");
			exit(1);
		}
	}
	return 0;
}
