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

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 20
#endif

int NUM_PROCS = 8;
int rows_per_blk;
int w_X, w_Y;
char **w;
char *arr;
char **neww;
char *arr2;

void wait_all_children() {
	while (wait(NULL) > 0)
		;
}

void set_real_procs() {
	/*If the number of rows or cols are less than NUM_PROCS
	 * then change NUM_PROCS*/
	if (w_X <= NUM_PROCS && w_X <= w_Y)
		NUM_PROCS = w_X;
	else if (w_Y <= NUM_PROCS && w_Y <= w_X)
		NUM_PROCS = w_Y;

	/*Falling back to 2 processes when distributing equal work in top N-1 processes is complex*/
	if (w_Y > 8 && w_Y < 30)
		NUM_PROCS = 2;

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

void init_new(int myid) {
	int i, j;

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

void init_seq(int X, int Y) {
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

int neighborcount_seq(int x, int y) {
	int count = 0;

	if ((x < 0) || (x >= w_X)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}
	if ((y < 0) || (y >= w_Y)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}

	if (w_X == 1 && w_Y == 1) {
		count = 0;
	} else if (w_X == 1) {
		if (y == 0) {
			count = w[y + 1][x];
		} else if (y == w_Y - 1) {
			count = w[y - 1][x];
		} else {
			count = w[y - 1][x] + w[y + 1][x];
		}
	} else if (w_Y == 1) {
		if (x == 0) {
			count = w[y][x + 1];
		} else if (x == w_X - 1) {
			count = w[y][x - 1];
		} else {
			count = w[y][x + 1] + w[y][x - 1];
		}
	} else {
		printf("WRONG!!\n");
	}

	return count;
}

int neighborcount(int x, int y) {
	int count = 0;

	if ((x < 0) || (x >= w_X)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}
	if ((y <= 0) || (y > rows_per_blk)) {
		printf("neighborcount: (%d %d) out of bound (0..%d, 0..%d).\n", x, y, w_X, w_Y);
		exit(0);
	}

	if (x == 0) {
		count = w[y - 1][x] + w[y + 1][x] + w[y - 1][x + 1] + w[y][x + 1] + w[y + 1][x + 1];
	} else if (x == w_X - 1) {
		count = w[y - 1][x] + w[y + 1][x] + w[y - 1][x - 1] + w[y][x - 1] + w[y + 1][x - 1];
	} else {
		count = w[y - 1][x - 1] + w[y][x - 1] + w[y + 1][x - 1] + w[y - 1][x] + w[y + 1][x] + w[y - 1][x + 1] + w[y][x + 1] + w[y + 1][x + 1];
	}

	return count;
}

int main(int argc, char *argv[]) {
	int x, y;
	int iter = 0;
	int c;
	int init_count = -1;
	int count;
	if (setvbuf(stdout, NULL, _IONBF, BUFSIZ))
		printf("Usage: ./a.out w_X w_Y\n");
	if (argc == 1) {
		printf("Usage: ./a.out w_X w_Y\n");
		exit(0);
	}

	w_X = atoi(argv[1]), w_Y = atoi(argv[2]);

	set_real_procs();
	if (NUM_PROCS > 1) {
		rows_per_blk = ceil(w_Y * 1.0f / NUM_PROCS);

		w = malloc((rows_per_blk + 2) * sizeof(*w));
		neww = malloc((rows_per_blk + 2) * sizeof(*neww));
		arr = malloc((rows_per_blk + 2) * w_X * sizeof(char));
		arr2 = malloc((rows_per_blk + 2) * w_X * sizeof(char));
		for (int i = 0; i < (rows_per_blk + 2); ++i) {
			w[i] = &arr[i * w_X];
			neww[i] = &arr2[i * w_X];
		}

		/*Condition when there is only one row or column
		 * must be handled separately all others are handled*/
		int num_pipe = NUM_PROCS - 1;
		int pipe_down[num_pipe][2];
		int pipe_up[num_pipe][2];

		/*Since there is an extra handler processes*/
		int count_down[NUM_PROCS][2];
		int count_up[NUM_PROCS][2];
		int file_write[NUM_PROCS][2];
		int debug_write[NUM_PROCS][2];
		int sp_pipe_down[2];
		int sp_pipe_up[2];
		for (int i = 0; i < NUM_PROCS; i++) {
			if (i < num_pipe) {
				if (pipe(pipe_down[i]) == -1) {
					perror("Pipe creation : ");
				}
				if (pipe(pipe_up[i]) == -1) {
					perror("Pipe creation : ");
				}
			}
			if (pipe(count_down[i]) == -1) {
				perror("Pipe creation : ");
			}
			if (pipe(count_up[i]) == -1) {
				perror("Pipe creation : ");
			}
			if (pipe(file_write[i]) == -1) {
				perror("Pipe creation : ");
			}
			if (pipe(debug_write[i]) == -1) {
				perror("Pipe creation : ");
			}
		}

		if (pipe(sp_pipe_down) == -1) {
			perror("Pipe creation : ");
		}
		if (pipe(sp_pipe_up) == -1) {
			perror("Pipe creation : ");
		}
		for (int i = 0; i < NUM_PROCS; i++) {
			init_new(i);

			/*############################################################################################################*/
			if (i == 0) {
				if (fork() == 0) {
					/*First process so reads only bottom row*/
					int myid = i;
					int x, y;
					for (int i = 0; i < num_pipe; i++) {
						/*Close unnecessary pipes*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid) {
							close(pipe_down[i][0]);
							close(pipe_down[i][1]);
							close(pipe_up[i][0]);
							close(pipe_up[i][1]);
						}
					}
					for (int i = 0; i < NUM_PROCS; i++) {
						/*Dedicated pipes to handlers*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid) {
							close(count_down[i][0]);
							close(count_down[i][1]);
							close(count_up[i][0]);
							close(count_up[i][1]);
							close(file_write[i][0]);
							close(file_write[i][1]);
							close(debug_write[i][0]);
							close(debug_write[i][1]);
						}
					}

					close(sp_pipe_up[0]);
					close(sp_pipe_up[1]);
					close(sp_pipe_down[0]);
					close(sp_pipe_down[1]);
					/*The init_count part before the loop*/
					int count = 0;
					int init_count = 0;
					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (w[y][x] == 1)
								count++;
						}
					}

					if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
						perror("Write START INIT: ");
					}

					if (read(count_up[myid][0], &init_count, sizeof(init_count)) != 4) {
						perror("Read START INIT: ");
					}



					count = init_count;

					if (DEBUG_LEVEL >= 10) {
						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
									perror("FileWrite START : ");
								}
							}
						}
					}

					/*Communicate with next process*/

					for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {
						for (int i = 0; i < w_X; i++) {
							if (write(pipe_down[myid][1], &w[rows_per_blk][i], sizeof(w[rows_per_blk][i])) != 1) {
								perror("Write START : ");
							}
						}

						for (int i = 0; i < w_X; i++) {
							if (read(pipe_up[myid][0], &w[rows_per_blk + 1][i], sizeof(w[rows_per_blk + 1][i])) != 1) {
								perror("Read START : ");
							}
						}

						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
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
							for (y = 1; y <= rows_per_blk; y++) {
								w[y][x] = neww[y][x];
								if (w[y][x] == 1)
									count++;
							}
						}

						/*Calculate the new pop*/
						if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
							perror("Write START INIT: ");
						}

						if (read(count_up[myid][0], &count, sizeof(count)) != 4) {
							perror("Read START INIT: ");
						}

						if (DEBUG_LEVEL >= 10) {
							for (x = 0; x < w_X; x++) {
								for (y = 1; y <= rows_per_blk; y++) {
									if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
										perror("FileWrite START : ");
									}
								}
							}
						}

					}

					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (write(file_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
								perror("FileWrite START : ");
							}
						}
					}

					close(pipe_down[myid][0]);
					close(pipe_down[myid][1]);
					close(pipe_up[myid][0]);
					close(pipe_up[myid][1]);
					close(count_down[myid][0]);
					close(count_down[myid][1]);
					close(count_up[myid][0]);
					close(count_up[myid][1]);

					close(file_write[myid][0]);
					close(file_write[myid][1]);
					close(debug_write[myid][0]);
					close(debug_write[myid][1]);

					exit(0);

				}
			}

			/*############################################################################################################*/
			else if (i == NUM_PROCS - 1) {
				if (fork() == 0) {
					/*Last process so reads only top row*/
					int myid = i;

					for (int i = 0; i < num_pipe; i++) {
						/*Close unnecessary pipes*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid - 1) {
							close(pipe_down[i][0]);
							close(pipe_down[i][1]);
							close(pipe_up[i][0]);
							close(pipe_up[i][1]);
						}
					}
					for (int i = 0; i < NUM_PROCS; i++) {
						/*Dedicated pipes to handlers*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid) {
							close(count_down[i][0]);
							close(count_down[i][1]);
							close(count_up[i][0]);
							close(count_up[i][1]);
							close(file_write[i][0]);
							close(file_write[i][1]);
							close(debug_write[i][0]);
							close(debug_write[i][1]);
						}
					}

					close(sp_pipe_up[0]);
					close(sp_pipe_up[1]);
					close(sp_pipe_down[0]);
					close(sp_pipe_down[1]);
					/*The init_count part before the loop*/
					int count = 0;
					int init_count = 0;
					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (myid * rows_per_blk + y <= w_Y) {
								if (w[y][x] == 1)
									count++;
							} else {
								break;
							}
						}
					}

					if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
						perror("Write END INIT: ");
					}

					if (read(count_up[myid][0], &init_count, sizeof(init_count)) != 4) {
						perror("Read END INIT: ");
					}

					count = init_count;
					if (DEBUG_LEVEL >= 10) {
						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (myid * rows_per_blk + y <= w_Y) {
									if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
										perror("FileWrite END : ");
									}
								} else {
									break;
								}
							}
						}
					}

					for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {
						/*Communicate with previous process*/
						for (int i = 0; i < w_X; i++) {
							if (write(pipe_up[myid - 1][1], &w[1][i], sizeof(w[rows_per_blk][i])) != 1) {
								perror("Write END : ");
							}
						}

						for (int i = 0; i < w_X; i++) {
							if (read(pipe_down[myid - 1][0], &w[0][i], sizeof(w[rows_per_blk + 1][i])) != 1) {
								perror("Read END : ");
							}
						}

						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (!(myid * rows_per_blk + y <= w_Y)) {
									w[y][x] = 0;
								}
							}
						}

						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {

								if (myid * rows_per_blk + y <= w_Y) {
									c = neighborcount(x, y); /* count neighbors */
									if (c <= 1)
										neww[y][x] = 0; /* die of loneliness */
									else if (c >= 4)
										neww[y][x] = 0; /* die of overpopulation */
									else if (c == 3)
										neww[y][x] = 1; /* becomes alive */
									else
										neww[y][x] = w[y][x]; /* c == 2, no change */
								} else {
									break;
								}

							}
						}

						/* copy the world, and count the current lives */
						count = 0;
						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (myid * rows_per_blk + y <= w_Y) {
									w[y][x] = neww[y][x];
									if (w[y][x] == 1)
										count++;
								} else {
									break;
								}
							}
						}

						/*Calculate the new pop*/
						if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
							perror("Write START INIT: ");
						}

						if (read(count_up[myid][0], &count, sizeof(count)) != 4) {
							perror("Read START INIT: ");
						}

						if (DEBUG_LEVEL >= 10) {
							for (x = 0; x < w_X; x++) {
								for (y = 1; y <= rows_per_blk; y++) {
									if (myid * rows_per_blk + y <= w_Y) {
										if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
											perror("FileWrite END : ");
										}
									} else {
										break;
									}
								}
							}
						}
					}

					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (myid * rows_per_blk + y <= w_Y) {
								if (write(file_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
									perror("FileWrite END : ");
								}
							} else {
								break;
							}
						}
					}

					close(pipe_down[myid - 1][0]);
					close(pipe_down[myid - 1][1]);
					close(pipe_up[myid - 1][0]);
					close(pipe_up[myid - 1][1]);
					close(count_down[myid][0]);
					close(count_down[myid][1]);
					close(count_up[myid][0]);
					close(count_up[myid][1]);

					close(file_write[myid][0]);
					close(file_write[myid][1]);
					close(debug_write[myid][0]);
					close(debug_write[myid][1]);

					exit(0);
				}
			}

			/*############################################################################################################*/
			else {
				if (fork() == 0) {
					/*Middle process so needs both top and bottom rows*/
					int myid = i;
					for (int i = 0; i < num_pipe; i++) {
						/*Close unnecessary pipes*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid - 1 && i != myid) {
							close(pipe_down[i][0]);
							close(pipe_down[i][1]);
							close(pipe_up[i][0]);
							close(pipe_up[i][1]);
							close(file_write[i][0]);
							close(file_write[i][1]);
						}
					}
					for (int i = 0; i < NUM_PROCS; i++) {
						/*Dedicated pipes to handlers*/
						/*Writes and Reads to pipe[i]*/
						if (i != myid) {
							close(count_down[i][0]);
							close(count_down[i][1]);
							close(count_up[i][0]);
							close(count_up[i][1]);
							close(debug_write[i][0]);
							close(debug_write[i][1]);
						}
					}

					close(sp_pipe_up[0]);
					close(sp_pipe_up[1]);
					close(sp_pipe_down[0]);
					close(sp_pipe_down[1]);

					/*The init_count part before the loop*/
					int count = 0;
					int init_count = 0;
					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (w[y][x] == 1)
								count++;
						}
					}

					if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
						perror("Write MID INIT: ");
					}

					if (read(count_up[myid][0], &init_count, sizeof(init_count)) != 4) {
						perror("Read MID INIT: ");
					}

					count = init_count;
					if (DEBUG_LEVEL >= 10) {
						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
									perror("FileWrite START : ");
								}
							}
						}
					}

					for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {
						/*Communicate with previous process*/
						for (int i = 0; i < w_X; i++) {
							if (write(pipe_up[myid - 1][1], &w[1][i], sizeof(w[1][i])) != 1) {
								perror("Write MID : ");
							}
						}

						for (int i = 0; i < w_X; i++) {
							if (read(pipe_down[myid - 1][0], &w[0][i], sizeof(w[0][i])) != 1) {
								perror("Read MID : ");
							}
						}

						/*Communicate with next process*/
						for (int i = 0; i < w_X; i++) {
							if (write(pipe_down[myid][1], &w[rows_per_blk][i], sizeof(w[rows_per_blk][i])) != 1) {
								perror("Write MID : ");
							}
						}

						for (int i = 0; i < w_X; i++) {
							if (read(pipe_up[myid][0], &w[rows_per_blk + 1][i], sizeof(w[rows_per_blk + 1][i])) != 1) {
								perror("Read MID : ");
							}
						}

						for (x = 0; x < w_X; x++) {
							for (y = 1; y <= rows_per_blk; y++) {
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
							for (y = 1; y <= rows_per_blk; y++) {
								w[y][x] = neww[y][x];
								if (w[y][x] == 1)
									count++;
							}
						}

						/*Calculate the new pop*/
						if (write(count_down[myid][1], &count, sizeof(count)) != 4) {
							perror("Write START INIT: ");
						}

						if (read(count_up[myid][0], &count, sizeof(count)) != 4) {
							perror("Read START INIT: ");
						}

						if (DEBUG_LEVEL >= 10) {
							for (x = 0; x < w_X; x++) {
								for (y = 1; y <= rows_per_blk; y++) {
									if (write(debug_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
										perror("FileWrite START : ");
									}
								}
							}
						}

					}

					for (x = 0; x < w_X; x++) {
						for (y = 1; y <= rows_per_blk; y++) {
							if (write(file_write[myid][1], &w[y][x], sizeof(w[y][x])) != 1) {
								perror("FileWrite MID : ");
							}
						}
					}

					close(pipe_down[myid][0]);
					close(pipe_down[myid][1]);
					close(pipe_up[myid][0]);
					close(pipe_up[myid][1]);
					close(pipe_down[myid - 1][0]);
					close(pipe_down[myid - 1][1]);
					close(pipe_up[myid - 1][0]);
					close(pipe_up[myid - 1][1]);
					close(count_down[myid][0]);
					close(count_down[myid][1]);
					close(count_up[myid][0]);
					close(count_up[myid][1]);

					close(file_write[myid][0]);
					close(file_write[myid][1]);
					close(debug_write[myid][0]);
					close(debug_write[myid][1]);

					exit(0);
				}
			}
		}

		/*############################################################################################################*/
		if (fork() == 0) {
			/*The count handler*/
			int count = 0;
			int temp;
			for (int i = 0; i < num_pipe; i++) {
				/*Close unnecessary pipes*/
				close(pipe_down[i][0]);
				close(pipe_down[i][1]);
				close(pipe_up[i][0]);
				close(pipe_up[i][1]);
			}
			for (int i = 0; i < NUM_PROCS; i++) {
				close(file_write[i][0]);
				close(file_write[i][1]);
				close(debug_write[i][0]);
				close(debug_write[i][1]);
			}

			for (int i = 0; i < NUM_PROCS; i++) {
				int temp = 0;
				if (read(count_down[i][0], &temp, sizeof(temp)) != 4) {
					perror("Read Count Handler: ");
				}
				count += temp;
			}
			printf("initial world, population count: %d\n", count);

			for (int i = 0; i < NUM_PROCS; i++) {
				if (write(count_up[i][1], &count, sizeof(count)) != 4) {
					perror("Write Count Handler: ");
				}
			}
			if (DEBUG_LEVEL >= 10) {
				if (write(sp_pipe_up[1], &count, sizeof(count)) != 4) {
					perror("Write Count Handler: ");
				}
			}
			init_count = count;
			/*Only for synchronizing
			 * The data is discarded*/
			if (read(sp_pipe_down[0], &temp, sizeof(temp)) != 4) {
				perror("Read DBG: ");
			}
			for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {
				int temp = 0;

				count = 0;
				for (int i = 0; i < NUM_PROCS; i++) {
					int temp = 0;
					if (read(count_down[i][0], &temp, sizeof(temp)) != 4) {
						perror("Read Count Handler: ");
					}
					count += temp;
				}

				for (int i = 0; i < NUM_PROCS; i++) {
					if (write(count_up[i][1], &count, sizeof(count)) != 4) {
						perror("Write Count Handler: ");
					}
				}

				printf("iter = %d, population count = %d\n", iter, count);
				if (DEBUG_LEVEL >= 10) {
					if (write(sp_pipe_up[1], &count, sizeof(count)) != 4) {
						perror("Write Count Handler: ");
					}

					/*Only for synchronizing
					 * The data is discarded*/
					if (read(sp_pipe_down[0], &temp, sizeof(temp)) != 4) {
						perror("Read DBG: ");
					}
				}
			}

			for (int i = 0; i < NUM_PROCS; i++) {
				/*Close unnecessary pipes*/
				/*Writes and Reads to pipe[i]*/
				close(count_down[i][0]);
				close(count_down[i][1]);
				close(count_up[i][0]);
				close(count_up[i][1]);
			}

			close(sp_pipe_up[0]);
			close(sp_pipe_up[1]);
			close(sp_pipe_down[0]);
			close(sp_pipe_down[1]);

			exit(0);
		}

		/*############################################################################################################*/

		if (fork() == 0) {
			/*Function to write to a file*/

			for (int i = 0; i < num_pipe; i++) {
				close(pipe_down[i][0]);
				close(pipe_down[i][1]);
				close(pipe_up[i][0]);
				close(pipe_up[i][1]);
			}
			for (int i = 0; i < NUM_PROCS; i++) {
				close(count_down[i][0]);
				close(count_down[i][1]);
				close(count_up[i][0]);
				close(count_up[i][1]);
				close(debug_write[i][0]);
				close(debug_write[i][1]);
			}

			close(sp_pipe_up[0]);
			close(sp_pipe_up[1]);
			close(sp_pipe_down[0]);
			close(sp_pipe_down[1]);
			FILE *fd;
			if ((fd = fopen("final_worldprocess.txt", "w")) != NULL) {
				for (x = 0; x < w_X; x++) {
					for (int i = 0; i < NUM_PROCS; i++) {
						if (i == NUM_PROCS - 1) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (i * rows_per_blk + y <= w_Y) {
									char c;
									if (read(file_write[i][0], &c, sizeof(c)) != 1) {
										perror("FileWrite Handler: ");
									}
									fprintf(fd, "%d", (int) c);
								} else {
									break;
								}
							}
						} else {
							for (y = 1; y <= rows_per_blk; y++) {
								char c;
								if (read(file_write[i][0], &c, sizeof(c)) != 1) {
									perror("FileWrite Handler: ");
								}
								fprintf(fd, "%d", (int) c);
							}
						}
					}
					fprintf(fd, "\n");
				}
			} else {
				printf("Can't open file final_worldprocess.txt\n");
				exit(1);
			}

			fflush(fd);
			for (int i = 0; i < NUM_PROCS; i++) {
				close(file_write[i][0]);
				close(file_write[i][1]);
			}
			exit(0);
		}

		/*############################################################################################################*/

		if (fork() == 0) {
			/*Function to write to screen for debug*/
			int count = 0;
			int init_count = 0;
			for (int i = 0; i < num_pipe; i++) {
				close(pipe_down[i][0]);
				close(pipe_down[i][1]);
				close(pipe_up[i][0]);
				close(pipe_up[i][1]);
			}
			for (int i = 0; i < NUM_PROCS; i++) {
				close(count_down[i][0]);
				close(count_down[i][1]);
				close(count_up[i][0]);
				close(count_up[i][1]);
				close(file_write[i][0]);
				close(file_write[i][1]);
			}

			if (DEBUG_LEVEL >= 10) {
				if (read(sp_pipe_up[0], &init_count, sizeof(init_count)) != 4) {
					perror("Read DBG INIT: ");
				}

				count = init_count;

				for (x = 0; x < w_X; x++) {
					for (int i = 0; i < NUM_PROCS; i++) {
						if (i == NUM_PROCS - 1) {
							for (y = 1; y <= rows_per_blk; y++) {
								if (i * rows_per_blk + y <= w_Y) {
									char c;
									if (read(debug_write[i][0], &c, sizeof(c)) != 1) {
										perror("FileWrite Handler: ");
									}
									printf("%d", (int) c);
								} else {
									break;
								}
							}
						} else {
							for (y = 1; y <= rows_per_blk; y++) {
								char c;
								if (read(debug_write[i][0], &c, sizeof(c)) != 1) {
									perror("FileWrite Handler: ");
								}
								printf("%d", (int) c);
							}
						}
					}
					printf("\n");
					fflush(0);
				}

				/*Synchronzing between count and debug here*/
				if (write(sp_pipe_down[1], &count, sizeof(count)) != 4) {
					perror("Write Count Handler: ");
				}

				for (iter = 0; (iter < 200) && (count < 50 * init_count) && (count > init_count / 50); iter++) {

					if (read(sp_pipe_up[0], &count, sizeof(count)) != 4) {
						perror("Read DBG: ");
					}

					for (x = 0; x < w_X; x++) {
						for (int i = 0; i < NUM_PROCS; i++) {
							if (i == NUM_PROCS - 1) {
								for (y = 1; y <= rows_per_blk; y++) {
									if (i * rows_per_blk + y <= w_Y) {
										char c;
										if (read(debug_write[i][0], &c, sizeof(c)) != 1) {
											perror("FileWrite Handler: ");
										}
										printf("%d", (int) c);
									} else {
										break;
									}
								}
							} else {
								for (y = 1; y <= rows_per_blk; y++) {
									char c;
									if (read(debug_write[i][0], &c, sizeof(c)) != 1) {
										perror("FileWrite Handler: ");
									}
									printf("%d", (int) c);
								}
							}
						}
						printf("\n");
						fflush(0);
					}

					/*Synchronzing between count and debug here*/
					if (write(sp_pipe_down[1], &count, sizeof(count)) != 4) {
						perror("Write Count Handler: ");
					}
				}
			}
			for (int i = 0; i < NUM_PROCS; i++) {
				close(debug_write[i][0]);
				close(debug_write[i][1]);
			}

			close(sp_pipe_up[0]);
			close(sp_pipe_up[1]);
			close(sp_pipe_down[0]);
			close(sp_pipe_down[1]);
			exit(0);
		}

		/*############################################################################################################*/
		for (int i = 0; i < num_pipe; i++) {
			close(pipe_down[i][0]);
			close(pipe_down[i][1]);
			close(pipe_up[i][0]);
			close(pipe_up[i][1]);
		}
		for (int i = 0; i < NUM_PROCS; i++) {
			close(count_down[i][0]);
			close(count_down[i][1]);
			close(count_up[i][0]);
			close(count_up[i][1]);
			close(file_write[i][0]);
			close(file_write[i][1]);
		}

		close(sp_pipe_up[0]);
		close(sp_pipe_up[1]);
		close(sp_pipe_down[0]);
		close(sp_pipe_down[1]);
		wait_all_children();

	} else {
		/*Handling zero/negative processes*/
		if (NUM_PROCS <= 0) {
			printf("initial world, population count: %d\n", 0);
			exit(0);
		}

		w = malloc(w_Y * sizeof(*w));
		neww = malloc(w_Y * sizeof(*neww));
		arr = malloc(w_Y * w_X * sizeof(char));
		arr2 = malloc(w_Y * w_X * sizeof(char));
		for (int i = 0; i < w_Y; ++i) {
			w[i] = &arr[i * w_X];
			neww[i] = &arr2[i * w_X];
		}

		/*Handling single process as a sequential process*/
		init_seq(atoi(argv[1]), atoi(argv[2]));

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
					c = neighborcount_seq(x, y); /* count neighbors */
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
		if ((fd = fopen("final_worldprocess.txt", "w")) != NULL) {
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
	}

	return 0;
}
