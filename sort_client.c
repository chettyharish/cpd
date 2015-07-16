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
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#define NAME_LEN 1000
#define NUM_THREADS 16
#define MAXCONN 7
#define SWAP(x,y,lo) if (data[lo+y] < data[lo+x]) { long int tmp = data[lo+x]; data[lo+x] = data[lo+y]; data[lo+y] = tmp; }

long int *data;
long int *temp;
long int CURR_THREADS;
long int NUM_BLK = 2;
long int SIZE;
long int FSIZE;
long int RSIZE;
int myid[NUM_THREADS];
pthread_t tid[NUM_THREADS];

double start_time, end_time, orig_time;
struct timeval t;

void set_time(int timer) {
	gettimeofday(&t, NULL);
	if (timer == 0) {
		start_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 1) {
		end_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	} else if (timer == 2) {
		orig_time = 1.0e-6 * t.tv_usec + t.tv_sec;
	}
}

void write_long(int sockfd_client, char *num) {
	unsigned int size = sizeof(long int);
	int rlen = 0;
	int ret;

	while (rlen < size) {
		if ((ret = write(sockfd_client, (num + rlen), size - rlen)) == -1) {
			perror("read_long");
			exit(1);
		}

		if (ret <= 0) {
			perror("socket closed before consumption");
			exit(1);
		}
		rlen += ret;
	}
}

bool is_sorted(long int start, long int end) {
	int err_ct = 0;
	for (long int i = start; i <= end - 1; i++) {
		if (data[i] > data[i + 1]) {
			printf("ERROR : %ld\t%ld\t%ld\t%ld\n", data[i], data[i + 1], i, i + 1);
			err_ct++;
		}
	}
	if (err_ct > 0) {
		printf("err_ct = %d\n", err_ct);
		return false;
	}
	return true;
}

static __inline__ void sort8(long int * data, long int lo) {
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
static __inline__ void sort7(long int * data, long int lo) {
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

static __inline__ void sort6(long int * data, long int lo) {
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

static __inline__ void sort5(long int * data, long int lo) {

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

static __inline__ void sort4(long int * data, long int lo) {
	SWAP(0, 1, lo);
	SWAP(2, 3, lo);

	SWAP(0, 2, lo);
	SWAP(1, 3, lo);

	SWAP(1, 2, lo);
}

static __inline__ void sort3(long int * data, long int lo) {
	SWAP(1, 2, lo);

	SWAP(0, 2, lo);

	SWAP(0, 1, lo);
}

static __inline__ void sort2(long int * data, long int lo) {
	SWAP(0, 1, lo);
}

void merge(long int lo, long int mid, long int hi) {
	long int i, j, k;
//	memcpy(&temp[lo], &data[lo], (hi - lo + 1) * sizeof(long int));
	for (long int i = lo; i <= hi; i++) {
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

void mergesort(long int lo, long int hi) {
	if (hi <= lo)
		return;
	long int num = hi - lo + 1;
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

	long int mid = lo + (hi - lo) / 2;
	mergesort(lo, mid);
	mergesort(mid + 1, hi);
//	if (!(data[mid] < data[mid + 1])) {
//		Merge only if data is sorted
	merge(lo, mid, hi);
//	}
}

void *mergesort_caller(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((SIZE * 1.0f) / NUM_THREADS);
	long int start = myid * num_ele;
	long int end = (((myid + 1) * num_ele - 1) < (SIZE - 1)) ? ((myid + 1) * num_ele - 1) : (SIZE - 1);
	mergesort(start, end);
}

void *k_way_merger_single(void *arg) {
	int myid = *(int *) arg;
	long int num_ele = ceil((SIZE * 1.0f) / CURR_THREADS);
	long int start1 = myid * num_ele;
	long int end1 = myid * num_ele + num_ele / 2 - 1;
	long int start2 = myid * num_ele + num_ele / 2;
	long int end2 = (myid + 1) * num_ele - 1;
	if (end2 >= SIZE) {
		end2 = SIZE - 1;
	}

	long int curr1 = start1, curr2 = start2;

//	printf("TID = %10d\t\tstart1 = %10d\tend1 = %10d\tstart2 = %10d\tend2 = %10d\tcurr1 = %10d\tcurr2 = %10d\n", myid, start1, end1, start2, end2, curr1, curr2);
	long int i = start1;
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

	for (long int i = start1; i <= end2; i++)
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

int main(int argc, char **argv) {
	FILE *log_file = fopen("log", "w+");

	set_time(2);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	/*Creating and connecting sockets here*/
//	int sockfd_client;
//	struct sockaddr_in saddr_client;
//
//	if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//		fprintf(log_file, "Socket failed!\n");
//		exit(1);
//	}
//
//	memset(&saddr_client, 0, sizeof(struct sockaddr_in));
//	saddr_client.sin_family = AF_INET;
//	saddr_client.sin_port = htons((short) atoi(argv[2]));
//	saddr_client.sin_addr.s_addr = inet_addr(argv[1]);
//
//	if (connect(sockfd_client, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1) {
//		fprintf(log_file, "Connect failed!\n");
//		exit(1);
//	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 1 STARTED*/
	char outfile[100];
	FILE *in_file = fopen("temp", "r");
	if (!in_file) {
		printf("Input file missing\n");
		exit(1);
	}

	sprintf(outfile, "temp_lvl%d", 0);
	FILE *out_file = fopen(outfile, "w+");
	if (!out_file) {
		printf("Unable to create output file\n");
		exit(1);
	}

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
	if (data == NULL || temp == NULL) {
		perror("Malloc :");
		exit(1);
	}

	for (int blk = 0; blk < NUM_BLK; blk++) {
		printf("\nStarting with BLK = %d\n", blk);
		set_time(0);

		FILE *in_file = fopen("temp", "r");
		fseek(in_file, blk * RSIZE, SEEK_SET);
		long int count = 0;
		long int buf_num = BUFSIZ / 8;
		bool flag = false;
		printf("Here\n");
		while (flag == false) {
			if (count + buf_num >= SIZE) {
				flag = true;
				buf_num = (SIZE - count);
//				if (count % 10000 == 0)
				printf("Reading from count = %ld to count = %ld new_buf_num = %ld\n", count, count + buf_num, buf_num);
			}

			if (fread(&data[count], sizeof(long int), buf_num, in_file) == -1) {
				perror("fread");
				exit(1);
			}

			count += buf_num;
		}
		fclose(in_file);

		set_time(1);
		printf("Reading data completed \t Execution time =  %lf seconds\n", end_time - start_time);

		set_time(0);
		for (int i = 0; i < NUM_THREADS; i++) {
			myid[i] = i;
			pthread_create(&tid[i], NULL, &mergesort_caller, &myid[i]);
		}

		for (int i = 0; i < NUM_THREADS; i++) {
			pthread_join(tid[i], NULL);
		}
		set_time(1);
		printf("Split Merge Sort completed \t Execution time =  %lf seconds\n", end_time - start_time);

		set_time(0);
		k_way_single();
		set_time(1);
		printf("K-Way Merge completed \t Execution time =  %lf seconds\n", end_time - start_time);

		set_time(0);

		if (is_sorted(0, SIZE - 1) == true) {
			printf("Sorted correctly \n");
		} else {
			printf("Sorting error \n");
			exit(1);
		}

		set_time(1);
		printf("Testing completed \t Execution time =  %lf seconds\n", end_time - start_time);

		set_time(0);
		count = 0;
		buf_num = BUFSIZ / 8;
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

		set_time(1);
		printf("Writing completed to file %s\t Execution time =  %lf seconds\n", outfile, end_time - start_time);
	}
	fclose(out_file);

	set_time(1);
	printf("PHASE 1 Completed\t Execution time =  %lf seconds \n\n\n", end_time - orig_time);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 2 STARTED*/

	int CURR_BLK = NUM_BLK >> 1;
	int LVL = 0;
	while (CURR_BLK != 0) {
		set_time(0);
		char f1[100];
		char f2[100];
		char op[100];
		char remove_fn[100];
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
			long int count1 = 1;
			long int count2 = 1;
			long int num1, num2;
			long int loc1 = (2 * i) * (long int) NUM_ELE * 8;
			long int loc2 = (2 * i + 1) * (long int) NUM_ELE * 8;
			long int loc3 = (2 * i) * (long int) NUM_ELE * 8;
			fseek(first_file, loc1, SEEK_SET);
			fseek(second_file, loc2, SEEK_SET);
			fseek(op_file, loc3, SEEK_SET);
			printf("ST : first_file = %ld\t second_file = %ld\t out_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(op_file), NUM_ELE);

			if (CURR_BLK != 1) {
				int ret = -1;
				if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
					printf("fread  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
					exit(1);
				}
				if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
					printf("fread  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
					exit(1);
				}

				while (true) {
					if (num1 <= num2) {
						if ((ret = fwrite(&num1, sizeof(long int), 1, op_file)) != 1) {
							printf("fwrite1 : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
							exit(1);
						}
						count1++;
						if (count1 > NUM_ELE)
							break;
						if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
							printf("fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
							exit(1);
						}
					} else {
						if ((ret = fwrite(&num2, sizeof(long int), 1, op_file)) != 1) {
							printf("fwrite2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
							exit(1);
						}
						count2++;

						if (count2 > NUM_ELE)
							break;
						if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
							printf("fread2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
							exit(1);
						}
					}
				}

				while (count1 <= NUM_ELE) {
					if ((ret = fwrite(&num1, sizeof(long int), 1, op_file)) != 1) {
						printf("while fwrite1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
						exit(1);
					}
					count1++;
					if (count1 > NUM_ELE)
						break;
					if ((ret = fread(&num1, sizeof(long int), 1, first_file)) != 1) {
						printf("while fread1  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
						exit(1);
					}
				}

				while (count2 <= NUM_ELE) {
					if ((ret = fwrite(&num2, sizeof(long int), 1, op_file)) != 1) {
						printf("while fwrite2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
						exit(1);
					}
					count2++;
					if (count2 > NUM_ELE)
						break;
					if ((ret = fread(&num2, sizeof(long int), 1, second_file)) != 1) {
						printf("while fread2  : ret = %d\tcount1 = %ld\tcount2 = %ld\n", ret, count1, count2);
						exit(1);
					}
				}
				printf("EN : first_file = %ld\t second_file = %ld\t out_file = %ld\t NUM_ELE = %ld\n", ftell(first_file), ftell(second_file), ftell(op_file), NUM_ELE);
			}
		}

		fclose(first_file);
		fclose(second_file);
		fclose(op_file);

		set_time(1);

		printf("Merge LVL = %d Completed\t Execution time =  %lf seconds \n\n\n", LVL + 1, end_time - start_time);

		LVL++;
		if (LVL != 0) {
			/*removing the old file out*/
			sprintf(remove_fn, "rm temp_lvl%d", LVL - 1);
			printf("Removing file command %s\n", remove_fn);
			if (system(remove_fn) == -1) {
				printf("Removing file failed");
			}

		}
		CURR_BLK = CURR_BLK >> 1;
	}

	/*Remove all temporary files*/
	if (system("rm -f temp_lvl*") == -1) {
		printf("Removing file failed");
	}

	set_time(1);
	printf("PHASE 2 Completed\t Execution time =  %lf seconds \n", end_time - orig_time);
	fprintf(log_file, "Completed correctly!\n");

	/*Cleaning the files here*/
	if (system("rm -f temp sort_client sort_client.c") == -1)
		perror("System");

}
