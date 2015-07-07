#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#define MAX_N 8192
int NUM_THREADS = 32;
int DEBUG_LEVEL = 0;

char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];
int *counters;
int w_X, w_Y;

void init1() {
	int i, j;
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

void *new_status_generator(void *arg) {
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

void *new_world_generator(void *arg) {
	int myid = *(int *) arg;
	int x, y;
	int rows_per_blk = ceil((w_Y * 1.0f) / NUM_THREADS);
	int start = myid * rows_per_blk;

	int count = 0;
	for (x = 0; x < w_X; x++) {
		for (y = 0; y < rows_per_blk; y++) {
			if (start + y >= w_Y)
				break;
			w[start + y][x] = neww[start + y][x];
			if (w[start + y][x] == 1)
				count++;
		}
	}
	counters[myid] = count;
}

int main(int argc, char *argv[]) {
	int x, y;
	int iter = 0;
	int c;
	int init_count;
	int count;

	if (argc < 3) {
		printf("Usage: ./thread w_X w_Y [options]\n");
		exit(0);
	}

	/* more than three parameters */
	for (int i = 1; i < argc; i++) {
		if (i == 1) {
			w_X = atoi(argv[1]);
		} else if (i == 2) {
			w_Y = atoi(argv[2]);
		} else if (strcmp(argv[i], "-d") == 0) {
			DEBUG_LEVEL = atoi(argv[++i]);
		}else if (strcmp(argv[i], "-n") == 0) {
			NUM_THREADS = atoi(argv[++i]);
		}
	}
	/* more than three parameters */
	counters= malloc(sizeof(int)*NUM_THREADS);
//	printf("w_X = %d\tw_Y = %d\tDEBUG_LEVEL = %d\t NUM_THREADS = %d\n", w_X, w_Y, DEBUG_LEVEL, NUM_THREADS);
	init1();

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
		count = 0;
		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &new_status_generator, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}

		/* copy the world, and count the current lives */

		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &new_world_generator, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			count += counters[i];
		}

		printf("iter = %d, population count = %d\n", iter, count);
		if (DEBUG_LEVEL > 10)
			print_world();
	}

	{
		FILE *fd;
		if ((fd = fopen("final_worldthread.txt", "w")) != NULL) {
			for (x = 0; x < w_X; x++) {
				for (y = 0; y < w_Y; y++) {
					fprintf(fd, "%d", (int) w[y][x]);
				}
				fprintf(fd, "\n");
			}
		} else {
			printf("Can't open file final_worldthread.txt\n");
			exit(1);
		}
	}
	return 0;
}
