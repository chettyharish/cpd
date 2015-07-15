#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/mman.h>
extern FILE *popen(const char *command, const char *modes);
extern int pclose(FILE *stream);

void tokenize(char *buffer_temp, char *exec_args[]) {
	int counter = 0;
	char *token = strtok(buffer_temp, " ");
	while (token != NULL) {
		exec_args[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}
	exec_args[counter] = NULL;

//	for (int i = 0; i <= counter; i++)
//		printf("%s\t", exec_args[i]);
//	printf("\n");
}

int main(int argc, char **argv) {
//	time dd if=../cop5570a/test0 bs=32M  iflag=skip_bytes,count_bytes skip=8000000000 count=8000000000 | ssh m4 'cat > temop'
//	2915.009221
//	ssh m4 'echo Done > lvl_done'
//	while (true) {
//		if (access("test_file", F_OK) == -1) {
//			printf("File doesnt exist , sleeping\n");
//			fflush(0);
//			sleep(5);
//		} else {
//			printf("File found\n");
//			return 0;
//		}
//	}

	/*parallel file parallel scp*/
//	char scp_path[1000];
//	char buffer_temp[1000];
//	char *exec_args[100];
//	fgets(buffer_temp, sizeof(buffer_temp), popen("which scp", "r"));
//	sscanf(buffer_temp, "%s\n", scp_path);
//	int k = 1;
//	printf("Here\n");
//	for (int i = 0; i < 8; i++) {
//		if (k == 3 || k == 1)
//			k++;
//		if (fork() == 0) {
//			long int SIZE = 1000000;
//			long int num1[SIZE];
//			char fn[100];
//			sprintf(fn, "op%d", i);
//			FILE *file = fopen("../cop5570a/test0", "r");
//			FILE *file2 = fopen(fn, "w+");
//			fseek(file, (long int) i * 1000000000l * 8l, SEEK_SET);
//
//			printf("%d file started\n", i);
//			for (long int j = 0; j < 1000000000l / SIZE; j++) {
//				if (fread(&num1[0], sizeof(long int), SIZE, file) == -1)
//					perror("fread");
//				if (fwrite(&num1[0], sizeof(long int), SIZE, file2) == -1)
//					perror("fwrite");
//				fflush(0);
//			}
//			printf("%d file done\n", i);
//
//			if (i != 7) {
//				sprintf(buffer_temp, "%s op%d m%d:", scp_path, i, k);
//				printf("%s\n", buffer_temp);
//				fflush(0);
//				tokenize(buffer_temp, exec_args);
//				if (execv(exec_args[0], exec_args) == -1) {
//					printf("execv execution error!\n");
//				}
//			}
//			return 0;
//		}
//		printf("k = %d\n", k);
//		k++;
//	}
//
//	while (wait(NULL) > 0)
//		;
//	/*seq file parallel scp*/
//	char scp_path[1000];
//	char buffer_temp[1000];
//	char *exec_args[100];
//	fgets(buffer_temp, sizeof(buffer_temp), popen("which scp", "r"));
//	sscanf(buffer_temp, "%s\n", scp_path);
//	int k = 1;
//	printf("Here\n");
//	for (int i = 0; i < 8; i++) {
//		if (k == 3 || k == 1)
//			k++;
//		long int SIZE = 1000000;
//		long int num1[SIZE];
//		char fn[100];
//		sprintf(fn, "op%d", i);
//		FILE *file = fopen("../cop5570a/test0", "r");
//		FILE *file2 = fopen(fn, "w+");
//		fseek(file, (long int) i * 1000000000l * 8l, SEEK_SET);
//
//		printf("%d file started\n", i);
//		for (long int j = 0; j < 1000000000l / SIZE; j++) {
//			if (fread(&num1[0], sizeof(long int), SIZE, file) == -1)
//				perror("fread");
//			if (fwrite(&num1[0], sizeof(long int), SIZE, file2) == -1)
//				perror("fwrite");
//			fflush(0);
//		}
//		printf("%d file done\n", i);
//
//		if (fork() == 0) {
//			if (i != 7) {
//				sprintf(buffer_temp, "%s op%d m%d:", scp_path, i, k);
//				printf("%s\n", buffer_temp);
//				fflush(0);
//				tokenize(buffer_temp, exec_args);
//				if (execv(exec_args[0], exec_args) == -1) {
//					printf("execv execution error!\n");
//				}
//			}
//			return 0;
//		}
//		printf("k = %d\n", k);
//		k++;
//	}
//
//	while (wait(NULL) > 0)
//		;
	/*seq file parallel scp*/
	char scp_path[1000];
	char buffer_temp[1000];
	char *exec_args[100];
	fgets(buffer_temp, sizeof(buffer_temp), popen("which scp", "r"));
	sscanf(buffer_temp, "%s\n", scp_path);
	int k = 1;
	for (int i = 0; i < 8; i++) {
		if (k == 3 || k == 1)
			k++;
		long int SIZE = 1000000;
		long int num1[SIZE];
		char fn[100];
		sprintf(fn, "op%d", i);
		printf("%d file started\n", i);

		int fd1 = open("../cop5570a/test0", O_RDONLY);
		int fd2 = open(fn, O_RDWR | O_CREAT, 0600);
		long int *read = reinterpret_cast<long int *>(mmap(NULL, 64000000000l, PROT_READ, MAP_FILE |MAP_SHARED |MAP_POPULATE, fd1, 0));
		long int *write = reinterpret_cast<long int *>(mmap(NULL, 64000000000l , PROT_READ | PROT_WRITE, MAP_FILE |MAP_SHARED, fd2, 0));
		madvise(read, 64000000000l, MADV_SEQUENTIAL | MADV_WILLNEED | MADV_DONTDUMP);
		printf("%d file done\n", i);

		memcpy(write, read + (long int) i * 1000000000l * 8l, 1000000000l * 8l);

		if (fork() == 0) {
			if (i != 7) {
				sprintf(buffer_temp, "%s op%d m%d:", scp_path, i, k);
				printf("%s\n", buffer_temp);
				fflush(0);
				tokenize(buffer_temp, exec_args);
				if (execv(exec_args[0], exec_args) == -1) {
					printf("execv execution error!\n");
				}
			}
			return 0;
		}
		printf("k = %d\n", k);
		k++;
	}

	while (wait(NULL) > 0)
		;

}
