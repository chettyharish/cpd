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

//#define MIN(x, y) (y ^ ((x ^ y) & -(x < y)))
//#define MAX(x, y) (x ^ ((x ^ y) & -(x < y)))
//#define SWAP(x,y , lo) { int tmp = MIN(data[lo+x], data[lo+y]); data[lo+y] = MAX(data[lo+x], data[lo+y]); data[lo+x] = tmp; }
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

#define NUM_THREADS  16
long int *data;
long int *temp;
long int CURR_THREADS;
long int NUM_BLK;
long int SIZE;
long int FSIZE;
long int RSIZE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];

bool is_sorted(int start, int end) {
	int err_ct = 0;
	for (int i = start; i <= end - 1; i++) {
		if (data[i] > data[i + 1]) {
//			printf("ERROR : %ld\t%ld\t%d\t%d\n", data[i], data[i + 1], i, i + 1);
			err_ct++;
		}
	}
	if (err_ct > 0) {
		printf("err_ct = %d\n", err_ct);
		return false;
	}
	return true;
}

static __inline__ void sort8(long int * data, int lo) {

	SWAP(0, 1, lo);
	SWAP(2, 3, lo);
	SWAP(4, 5, lo);
	SWAP(6, 7, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);
	SWAP(4, 6, lo);
	SWAP(5, 7, lo);

	SWAP(1, 2, lo);
	SWAP(5, 6, lo);
	SWAP(0, 4, lo);
	SWAP(3, 7, lo);

	SWAP(1, 5, lo);
	SWAP(2, 6, lo);

	SWAP(1, 4, lo);
	SWAP(3, 6, lo);

	SWAP(2, 4, lo);
	SWAP(3, 5, lo);

	SWAP(3, 4, lo);
}
static __inline__ void sort7(long int * data, int lo) {
	SWAP(1, 2, lo);
	SWAP(3, 4, lo);
	SWAP(5, 6, lo);

	SWAP(0, 2, lo);
	SWAP(3, 5, lo);
	SWAP(4, 6, lo);

	SWAP(0, 1, lo);
	SWAP(4, 5, lo);
	SWAP(2, 6, lo);

	SWAP(0, 4, lo);
	SWAP(1, 5, lo);

	SWAP(0, 3, lo);
	SWAP(2, 5, lo);
	SWAP(1, 3, lo);
	SWAP(2, 4, lo);
	SWAP(2, 3, lo);
}

static __inline__ void sort6(long int * data, int lo) {
	//Parallelizable
	SWAP(1, 2, lo);
	SWAP(4, 5, lo);

	//Parallelizable
	SWAP(0, 2, lo);
	SWAP(3, 5, lo);

	//Parallelizable
	SWAP(0, 1, lo);
	SWAP(3, 4, lo);
	SWAP(2, 5, lo);

	//Parallelizable
	SWAP(0, 3, lo);
	SWAP(1, 4, lo);

	//Parallelizable
	SWAP(2, 4, lo);
	SWAP(1, 3, lo);

	//Parallelizable
	SWAP(2, 3, lo);
}

static __inline__ void sort5(long int * data, int lo) {

	SWAP(0, 1, lo);
	SWAP(3, 4, lo);

	SWAP(2, 4, lo);

	SWAP(2, 3, lo);
	SWAP(1, 4, lo);

	SWAP(0, 3, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);

	SWAP(1, 2, lo);
}

static __inline__ void sort4(long int * data, int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);

	SWAP(1, 2, lo);
}

static __inline__ void sort3(long int * data, int lo) {
	SWAP(1, 2, lo);

	SWAP(0, 2, lo);

	SWAP(0, 1, lo);
}

static __inline__ void sort2(long int * data, int lo) {
	SWAP(0, 1, lo);
}

void merge(int lo, int mid, int hi) {
	int i, j, k;
//	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
	for (int i = lo; i <= hi; i++) {
		temp[i] = data[i];
	}
	i = lo, j = mid + 1;

	for (k = lo; k <= hi; k++) {
		if (i > mid)
			data[k] = temp[j++];
		else if (j > hi)
			data[k] = temp[i++];
		else if (temp[i] < temp[j])
			data[k] = temp[i++];
		else
			data[k] = temp[j++];
	}
}

void mergesort(int lo, int hi) {
	if (hi <= lo)
		return;
	int num = hi - lo + 1;

	if (num <= 8) {
		switch (num) {
		case 0:
		case 1:
			return;
		case 2:
			sort2(data, lo);
			return;
		case 3:
			sort3(data, lo);
			return;
		case 4:
			sort4(data, lo);
			return;
		case 5:
			sort5(data, lo);
			return;
		case 6:
			sort6(data, lo);
			return;
		case 7:
			sort7(data, lo);
			return;
//		case 8:
//			sort8(data, lo);
//			return;
		}
	}

	int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
//	if (!(data[mid] < data[mid + 1])) {
//		Merge only if data is sorted
	merge(lo, mid, hi);
//	}
}

void *mergesort_caller(void *arg) {
	int myid = *(int *) arg;
	int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
	int start = myid * num_ele;
	int end = (((myid + 1) * num_ele - 1) < (SIZE - 1)) ? ((myid + 1) * num_ele - 1) : (SIZE - 1);
	mergesort(start, end);
}

int find_min(long int *vals) {
	int min = vals[0];
	int pos = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		if (vals[i] < min) {
			min = vals[i];
			pos = i;
		}
	}
	return pos;
}

void *k_way_merger_single(void *arg) {
	int myid = *(int *) arg;
	int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
	int start1 = myid * num_ele;
	int end1 = myid * num_ele + num_ele / 2 - 1;
	int start2 = myid * num_ele + num_ele / 2;
	int end2 = (myid + 1) * num_ele - 1;
	if (end2 >= SIZE) {
		end2 = SIZE - 1;
	}

	int curr1 = start1, curr2 = start2;

//	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);
	int i = start1;
	while (curr1 <= end1 && curr2 <= end2) {
		if (data[curr1] <= data[curr2])
			temp[i++] = data[curr1++];
		else
			temp[i++] = data[curr2++];
	}

	while (curr1 <= end1) {
		temp[i++] = data[curr1++];
	}

	while (curr2 <= end2) {
		temp[i++] = data[curr2++];
	}

	for (int i = start1; i <= end2; i++)
		data[i] = temp[i];
}

void k_way_single() {
	CURR_THREADS = NUM_THREADS >> 1;
	while (CURR_THREADS != 0) {
		for (int i = 0; i < CURR_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &k_way_merger_single, &myid[i]);
		}

		for (int i = 0; i < CURR_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}

		CURR_THREADS = CURR_THREADS >> 1;
	}

}

long int read_chunk(FILE *r_file, int pos, int CSIZE, int buf_num, long int END) {

	long int start = ftell(r_file) / 8;
	long int curr = ftell(r_file) / 8;
	int count = 0;
	bool flag = false;
	while (true) {
		curr = ftell(r_file) / 8;
		if (count >= CSIZE || flag == true) {
			return ((ftell(r_file) / 8) - start);
		}
		if (curr + buf_num >= END) {
			flag = true;
			buf_num = (END - curr);
		}

		if (fread(&data[pos + count], sizeof(long int), buf_num, r_file) == -1) {
			perror("fread");
			exit(1);
		}

		count += buf_num;
	}

	printf("read_chunk  OUT BAD  \n");

	return -1;
}

void write_chunk(FILE *out_file, int w_count, int CSIZE, int buf_num, long int END) {
	long int start = ftell(out_file) / 8;
	long int curr = ftell(out_file) / 8;

	int count = 0;
	bool flag = false;
	while (true) {
		curr = ftell(out_file) / 8;
		if (count >= CSIZE * 2 || flag == true)
			return;
		if (curr + buf_num >= END) {
			flag = true;
			buf_num = (END - curr);
		}
		if (fwrite(&temp[count], sizeof(long int), buf_num, out_file) == -1) {
			perror("fwrite");
			exit(1);
		}
		count += buf_num;
	}
	fflush(out_file);
}

int main(int argc, char **argv) {

	double start_time, end_time, orig_time;
	struct timeval t;
	char outfile[100];

	gettimeofday(&t, NULL);
	orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;

	if (argc < 3) {
		printf("./mysort input_file num_blk mode \n");
		exit(1);
	}
	FILE *in_file = fopen(argv[1], "r");
	if (!in_file) {
		printf("Input file missing\n");
		exit(0);
	}

	sprintf(outfile, "temp_lvl%d", 0);

	FILE *out_file = fopen(outfile, "w+");
	if (!out_file) {
		printf("Unable to create output file\n");
		exit(0);
	}

	NUM_BLK = atoi(argv[2]);
	int count = 0;
	bool flag = false;

	fseek(in_file, 0, SEEK_END);
	FSIZE = ftell(in_file);
	fseek(in_file, 0, SEEK_SET);
	fclose(in_file);
	SIZE = (FSIZE / 8) / NUM_BLK;
	RSIZE = FSIZE / NUM_BLK;

	printf("RSIZE = %ld \tBUFSIZ = %d\t NUM_THREADS = %d\t FSIZE = %ld\t SIZE = %ld\t NUM_BLK = %ld \t FITTING %f\n", RSIZE, BUFSIZ, NUM_THREADS, FSIZE, SIZE, NUM_BLK,
			(SIZE * 1.0f) / (NUM_THREADS * 1.0f));

	data = malloc(sizeof(long int) * SIZE);
	temp = malloc(sizeof(long int) * SIZE);
	for (int i = 0; i < SIZE; i++) {
		data[i] = -100;
		temp[i] = -100;
	}

	if (data == NULL && temp == NULL) {
		perror("Malloc :");
		exit(1);
	}

	for (int blk = 0; blk < NUM_BLK; blk++) {
		printf("Here\n");
		printf("\nStarting with BLK = %d\n", blk);
		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;

		FILE *in_file = fopen(argv[1], "r");
		fseek(in_file, blk * RSIZE, SEEK_SET);

		count = 0;
		flag = false;
		int buf_num = BUFSIZ / 8;
		while (flag == false) {
			if (count + buf_num >= SIZE) {
				flag = true;
				buf_num = (SIZE - count);
			}
			if (fread(&data[count], sizeof(long int), buf_num, in_file) == -1) {
				perror("fread");
				exit(1);
			}
			count += buf_num;
		}
		fclose(in_file);

		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		printf("Reading data completed \t Execution time =  %lf seconds\n", end_time - start_time);

		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}
		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;

		printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);

		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		k_way_single();
		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);

		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;

		if (is_sorted(0, SIZE - 1) == true) {
			printf("Sorted correctly \n");
		} else {
			printf("Sorting error \n");
			exit(1);
		}

		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		printf("Testing completed \t Execution time =  %lf seconds\n", end_time - start_time);

		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;

		buf_num = BUFSIZ / 8;
		count = 0;
		flag = false;
		while (flag == false) {
			if (count + buf_num >= SIZE) {
				flag = true;
				buf_num = (SIZE - count);
			}

			if (fwrite(&data[count], sizeof(long int), buf_num, out_file) == -1) {
				perror("fwrite");
				exit(1);
			}
			count += buf_num;
		}
		fflush(out_file);

		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		printf("Writing completed to file %s\t Execution time =  %lf seconds\n", outfile, end_time - start_time);

	}

	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("PHASE 1 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);

	for (int i = 0; i < SIZE; i++) {
		data[i] = -100;
		temp[i] = -100;
	}

	fclose(out_file);

	int CURR_BLK = NUM_BLK >> 1;
	int LVL = 0;
	while (CURR_BLK != 0) {
		gettimeofday(&t, NULL);
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
		char f1[100];
		char f2[100];
		char op[100];
		char remove[100];
		sprintf(f1, "temp_lvl%d", LVL);
		sprintf(f2, "temp_lvl%d", LVL);
		if (CURR_BLK != 1) {
			sprintf(op, "temp_lvl%d", LVL + 1);
		} else {
			sprintf(op, "answer");
		}
		FILE *first_file = fopen(f1, "r");
		FILE *second_file = fopen(f2, "r");
		FILE *op_file = fopen(op, "wb+");
		long int NUM_ELE = ((long int) FSIZE / 8) / ((long int) NUM_BLK / (1 << LVL));

		for (int i = 0; i < CURR_BLK; i++) {
			long int loc1 = (2 * i) * (long int) NUM_ELE * 8;
			long int loc2 = (2 * i + 1) * (long int) NUM_ELE * 8;
			long int loc3 = (2 * i) * (long int) NUM_ELE * 8;
			fseek(first_file, loc1, SEEK_SET);
			fseek(second_file, loc2, SEEK_SET);
			fseek(op_file, loc3, SEEK_SET);
			printf("ST : first_file = %15ld\t second_file = %15ld\t out_file = %15ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(out_file), NUM_ELE);

			int buf_num = BUFSIZ / 8;
			int CSIZE = 10 * buf_num;
			long int END1 = loc1 / 8 + NUM_ELE;
			long int END2 = loc2 / 8 + NUM_ELE;
			long int END3 = END2;

			long int r_count1 = 0, r_count2 = 0, w_count = 0, total_count1 = 0, total_count2 = 0;
			long int start1 = 0;
			long int start2 = CSIZE;
			long int end1 = read_chunk(first_file, start1, CSIZE, buf_num, END1);
			long int end2 = read_chunk(second_file, start2, CSIZE, buf_num, END2);

			if (CURR_BLK != 1) {
				while (true) {
					if (data[start1] <= data[start2]) {
						temp[w_count++] = data[start1];
						start1++;
						r_count1++;
						total_count1++;
						if (r_count1 == end1) {
							if (end1 < CSIZE) {
								/*All chunks consumed by 1*/
								break;
							} else {
								/*Read next chunk*/
								start1 = 0;
								r_count1 = 0;
								end1 = read_chunk(first_file, start1, CSIZE, buf_num, END1);
							}
						}
					} else {
						temp[w_count++] = data[start2];
						start2++;
						r_count2++;
						total_count2++;
						if (r_count2 == end2) {
							if (end2 < CSIZE) {
								/*All chunks consumed by 2*/
								break;
							} else {
								/*Read next chunk*/

								start2 = CSIZE;
								r_count2 = 0;

								end2 = read_chunk(second_file, start2, CSIZE, buf_num, END2);

							}
						}
					}

					if (w_count == CSIZE * 2) {
						/*temp has two sorted chunk so write it back*/
						write_chunk(op_file, w_count, CSIZE, buf_num, END3);
						w_count = 0;
					}
				}

				while (total_count1 < NUM_ELE) {
					temp[w_count++] = data[start1];
					start1++;
					r_count1++;
					total_count1++;
					if (r_count1 == end1) {
						if (end1 < CSIZE) {
							/*All chunks consumed by 1*/
							break;
						} else {
							/*Read next chunk*/
							start1 = 0;
							r_count1 = 0;

							end1 = read_chunk(first_file, start1, CSIZE, buf_num, END1);
						}
					}

					if (w_count == CSIZE * 2) {
						/*temp has two sorted chunk so write it back*/
						write_chunk(op_file, w_count, CSIZE, buf_num, END3);
						w_count = 0;
					}
				}

				while (total_count2 < NUM_ELE) {
					temp[w_count++] = data[start2];
					start2++;
					r_count2++;
					total_count2++;
					if (r_count2 == end2) {
						if (end2 < CSIZE) {
							/*All chunks consumed by 2*/
							break;
						} else {
							/*Read next chunk*/
							start2 = CSIZE;
							r_count2 = 0;
							end2 = read_chunk(second_file, start2, CSIZE, buf_num, END2);

						}
					}
					if (w_count == CSIZE * 2) {
						/*temp has two sorted chunk so write it back*/
						write_chunk(op_file, w_count, CSIZE, buf_num, END3);
						w_count = 0;
					}
				}

				if (w_count > 0) {
					write_chunk(op_file, w_count, CSIZE, buf_num, END3);

				}
			} else {
				long int all_count = 0;
				while (true) {
					if (data[start1] <= data[start2]) {
						if (all_count % 1000 == 0)
							fprintf(op_file, "%10d\t%20ld\n", (int) all_count, data[start1]);
						start1++;
						r_count1++;
						all_count++;
						total_count1++;
						if (r_count1 == end1) {
							if (end1 < CSIZE) {
								/*All chunks consumed by 1*/
								break;
							} else {
								/*Read next chunk*/
								start1 = 0;
								r_count1 = 0;

								end1 = read_chunk(first_file, start1, CSIZE, buf_num, END1);

							}
						}
					} else {
						if (all_count % 1000 == 0)
							fprintf(op_file, "%10d\t%20ld\n", (int) all_count, data[start2]);
						start2++;
						r_count2++;
						all_count++;
						total_count2++;
						if (r_count2 == end2) {
							if (end2 < CSIZE) {
								/*All chunks consumed by 2*/
								break;
							} else {
								/*Read next chunk*/
								start2 = CSIZE;
								r_count2 = 0;

								end2 = read_chunk(second_file, start2, CSIZE, buf_num, END2);

							}
						}
					}
				}

				while (total_count1 < NUM_ELE) {
					if (all_count % 1000 == 0)
						fprintf(op_file, "%10d\t%20ld\n", (int) all_count, data[start1]);
					start1++;
					r_count1++;
					all_count++;
					total_count1++;
					if (r_count1 == end1) {
						if (end1 < CSIZE) {
							/*All chunks consumed by 1*/
							break;
						} else {
							/*Read next chunk*/start1 = 0;
							r_count1 = 0;

							end1 = read_chunk(first_file, start1, CSIZE, buf_num, END1);

						}
					}
				}

				while (total_count2 < NUM_ELE) {
					if (all_count % 1000 == 0)
						fprintf(op_file, "%10d\t%20ld\n", (int) all_count, data[start2]);
					start2++;
					r_count2++;
					all_count++;
					total_count2++;
					if (r_count2 == end2) {
						if (end2 < CSIZE) {
							/*All chunks consumed by 2*/
							break;
						} else {
							/*Read next chunk*/
							start2 = CSIZE;
							r_count2 = 0;
							end2 = read_chunk(second_file, start2, CSIZE, buf_num, END2);

						}
					}
				}
			}
			printf("EN : first_file = %15ld\t second_file = %15ld\t out_file = %15ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(op_file), NUM_ELE);

		}

		fclose(first_file);
		fclose(second_file);
		fclose(op_file);

		gettimeofday(&t, NULL);
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;

		printf("Merge LVL = %d Completed\t Execution time =  %lf seconds \n\n\n", LVL + 1, end_time - start_time);

		LVL++;
		if (LVL != 0 && LVL != 1) {
			/*removing the old file out*/
			sprintf(remove, "rm temp_lvl%d", LVL - 2);
			if (system(remove) == -1) {
				printf("Removing file failed");
			}

		}
		CURR_BLK = CURR_BLK >> 1;
	}

//	/*Remove all temporary files*/
	if (system("rm temp_lvl*") == -1) {
		printf("Removing file failed");
	}

	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("PHASE 2 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);

	free(data);
	free(temp);
	gettimeofday(&t, NULL);
	end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	printf("Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
	return 0;
}
