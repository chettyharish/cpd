#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#define NUM_PROCS 32

int w_X, w_Y;

void wait_all_children() {
	while (wait(NULL) > 0)
		;
}

void init_new(int myid) {
	int i, j;
	int rows_per_blk = ceil(w_Y * 1.0f / NUM_PROCS);

	char w[rows_per_blk + 2][w_X];
	char neww[rows_per_blk + 2][w_X];
	for (j = 0; j < rows_per_blk + 2; j++)
		for (i = 0; i < w_X; i++)
			w[j][i] = 0;

	if (myid == 0) {
		/*Fill with 1's for first process*/
		for (i = 0; i < w_X; i++)
			w[1][i] = 1;
	}
	for (i = 1; i <= rows_per_blk; i++)
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
	int init_count = -1;
	int count;

	if (argc == 1) {
		printf("Usage: ./a.out w_X w_Y\n");
		exit(0);
	}

	w_X = atoi(argv[1]), w_Y = atoi(argv[2]);

	int num_pipe = NUM_PROCS - 1;
	int pipe_down[num_pipe][2];
	int pipe_up[num_pipe][2];

	/*Since there is an extra handler process*/
	int count_down[NUM_PROCS][2];
	int count_up[NUM_PROCS][2];
	for (int i = 0; i < NUM_PROCS; i++) {
		if (i < num_pipe) {
			pipe(pipe_down[i]);
			pipe(pipe_up[i]);
		}
		pipe(count_down[i]);
		pipe(count_up[i]);

	}

	for (int i = 0; i < NUM_PROCS; i++) {
		init_new(i);
		if (i == 0) {
			if (fork() == 0) {
				/*First process so reads only bottom row*/
				int myid = i;
				for (int i = 0; i < num_pipe; i++) {
					/*Close unnecessary pipes*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid) {
						pipe(pipe_down[i]);
						pipe(pipe_up[i]);
					}
				}
				for (int i = 0; i < NUM_PROCS; i++) {
					/*Dedicated pipes to handlers*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid) {
						pipe(count_down[i]);
						pipe(count_up[i]);
					}
				}

			}
		} else if (i == NUM_PROCS - 1) {
			if (fork() == 0) {
				/*Last process so reads only top row*/
				int myid = i;

				for (int i = 0; i < num_pipe; i++) {
					/*Close unnecessary pipes*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid - 1) {
						pipe(pipe_down[i]);
						pipe(pipe_up[i]);
					}
				}
				for (int i = 0; i < NUM_PROCS; i++) {
					/*Dedicated pipes to handlers*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid) {
						pipe(count_down[i]);
						pipe(count_up[i]);
					}
				}
			}
		} else {
			if (fork() == 0) {
				/*Middle process so needs both top and bottom rows*/
				int myid = i;

				for (int i = 0; i < num_pipe; i++) {
					/*Close unnecessary pipes*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid - 1 && i != myid) {
						pipe(pipe_down[i]);
						pipe(pipe_up[i]);
					}
				}
				for (int i = 0; i < NUM_PROCS; i++) {
					/*Dedicated pipes to handlers*/
					/*Writes and Reads to pipe[i]*/
					if (i != myid) {
						pipe(count_down[i]);
						pipe(count_up[i]);
					}
				}
			}
		}
	}

	if (fork() == 0) {
	}

	wait_all_children();

//	c = 0;
//	for (x = 0; x < w_X; x++) {
//		for (y = 0; y < w_Y; y++) {
//			if (w[y][x] == 1)
//				c++;
//		}
//	}
//
//	init_count = c;
//	count = init_count;

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

	{
		FILE *fd;
		if ((fd = fopen("final_worldseq.txt", "w")) != NULL) {
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
