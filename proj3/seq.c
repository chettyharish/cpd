/*
 * Created 06/20/2015 by Xin Yuan for the COP5570 class
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_N 8192
#define BOUND_CHECK 0
int DEBUG_LEVEL = 0;

char w[MAX_N][MAX_N];
char neww[MAX_N][MAX_N];

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
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}
	if ((y < 0) || (y >= w_Y)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}

	if (x == 0) {
		if (y == 0) {
			if (BOUND_CHECK == 1)
				printf("(%d,%d)  (%d,%d)  (%d,%d)  \n", y, x + 1, y + 1, x, y + 1, x + 1);

			count = w[y][x + 1] + w[y + 1][x] + w[y + 1][x + 1];
		} else if (y == w_Y - 1) {
			if (BOUND_CHECK == 1)
				printf("(%d,%d)  (%d,%d)  (%d,%d)  \n", y, x + 1, y - 1, x, y - 1, x + 1);

			count = w[y][x + 1] + w[y - 1][x] + w[y - 1][x + 1];
		} else {
			if (BOUND_CHECK == 1)
				printf("(%d,%d)  (%d,%d)  (%d,%d)  (%d,%d)  (%d,%d)  \n", y - 1, x, y + 1, x, y - 1, x + 1, y, x + 1, y + 1, x + 1);

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

	if (argc < 3) {
		printf("Usage: ./seq w_X w_Y [options]\n");
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
		}
	}
	/* more than three parameters */
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

	for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {

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

	FILE *fd;
	if ((fd = fopen("final_worldseq.txt", "w")) != NULL) {
		for (x = 0; x < w_X; x++) {
			for (y = 0; y < w_Y; y++) {
				fprintf(fd, "%d", (int) w[y][x]);
			}
			fprintf(fd, "\n");
		}
	} else {
		printf("Can't open file final_worldseq.txt\n");
		exit(1);
	}

	return 0;
}
