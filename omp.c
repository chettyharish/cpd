#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_N 8192

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];

int w_X, w_Y;
int CHUNK_SIZE;
void init1(int X, int Y) {
	int i, j;
	w_X = X, w_Y = Y;

	CHUNK_SIZE = ceil(((w_X < w_Y) ? w_X : w_Y) / 200.0f);
	printf("CHUNK_SIZE = %d\n", CHUNK_SIZE);

#pragma omp parallel for private(i,j) shared(w,w_X,w_Y) schedule(static , CHUNK_SIZE) collapse(2)
	for (i = 0; i < w_X; i++)
		for (j = 0; j < w_Y; j++)
			w[j][i] = 0;

#pragma omp parallel for private(i) shared(w,w_X) schedule(static , CHUNK_SIZE)
	for (i = 0; i < w_X; i++)
		w[0][i] = 1;

#pragma omp parallel for private(i) shared(w,w_Y) schedule(static , CHUNK_SIZE)
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
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}
	if ((y < 0) || (y >= w_Y)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
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
#pragma omp parallel for private(x,y) shared(w,w_X,w_Y) reduction(+:c) schedule(static , CHUNK_SIZE) collapse(2)
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

	for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {
#pragma omp parallel for private(x,y,c) shared(neww,w,w_X,w_Y) schedule(static , CHUNK_SIZE) collapse(2)
		for (x = 0; x < w_X; x++) {
			for (y = 0; y < w_Y; y++) {
				c = neighborcount(x, y); /* count neighbors */
				if (c <= 1)
					neww[y][x] = 0; /* die of loneliness */
				else if (c >= 4)
					neww[y][x] = 0; /* die of overpopulation */
				else if (c == 3)
					neww[y][x] = 1; /* becomes alive */
				else
					neww[y][x] = w[y][x]; /* c == 2, no change */
			}
		}

		/* copy the world, and count the current lives */
		count = 0;
#pragma omp parallel for private(x,y) shared(neww,w,w_X,w_Y) reduction(+:count) schedule(static , CHUNK_SIZE) collapse(2)
		for (x = 0; x < w_X; x++) {
			for (y = 0; y < w_Y; y++) {
				w[y][x] = neww[y][x];
				if (w[y][x] == 1) {
					count++;
				}
			}
		}
		printf("iter = %d, population count = %d\n", iter, count);
		if (DEBUG_LEVEL > 10)
			print_world();
	}

	{
		FILE *fd;
		if ((fd = fopen("final_worldomp.txt", "w")) != NULL) {
			for (x = 0; x < w_X; x++) {
				for (y = 0; y < w_Y; y++) {
					fprintf(fd, "%d", (int) w[y][x]);
				}
				fprintf(fd, "\n");
			}
		} else {
			printf("Can't open file final_worldomp.txt\n");
			exit(1);
		}
	}
	return 0;
}
