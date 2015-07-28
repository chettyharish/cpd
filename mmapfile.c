#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILEPATH "../cop5570a/test1"

int main(int argc, char *argv[]) {
	int fd;

	long int FILESIZE = 8000000000l * 8l;
	fd = open(FILEPATH, O_RDONLY);
	if (fd == -1) {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}

	long int *map = (long int *)mmap(0, FILESIZE, PROT_READ, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
	}
	printf("Done mmap\n");

	long int *data = malloc(sizeof(long) * 600000000l);
	long int *data2 = malloc(sizeof(long) * 600000000l);
	printf("Done malloc\n");
	for (long int i = 0; i < 600000000; ++i) {
		data[i] = map[i];
	}
	printf("Done data\n");

	for (long int i = 0; i < 600000000; ++i) {
		data2[i] = map[i + 600000000];
	}
	printf("Done data2\n");


	for(int i = 0 ; i < 10 ;i++){
		printf("d1 = %ld\n" , data[i]);
	}

	for(int i = 0 ; i < 10 ;i++){
		printf("d2 = %ld\n" , data2[i]);
	}

	if (munmap(map, FILESIZE) == -1) {
		perror("Error un-mmapping the file");
	}
	close(fd);
	return 0;
}
