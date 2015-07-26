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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>

#define MAXCONN 7
#define NAME_LEN 1000
char buffer_temp[NAME_LEN];
int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage ./mem_sort_server large_file_name machinefile\n");
		fflush(0);
		exit(1);
	}
	FILE *file = fopen(argv[2], "r");
	char mac_list[MAXCONN][1000];
	int num_machines = 0;
	char buffer_machines[NAME_LEN];
	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
		num_machines++;
	}

	if (num_machines < MAXCONN) {
		printf("Ensure that the file exists and there are exactly 7 machines which are active with 8GB memory free\n");
		exit(1);
	}
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < MAXCONN; i++) {
		/*Read the machines from machine file*/
		if (fscanf(file, "%s\n", mac_list[i]) == 0) {
			printf("Error reading machinefile = %s\n", argv[3]);
			exit(1);
		} else {
			printf("Machine %d = %s\n", i, mac_list[i]);
		}

	}

	printf("Done\n");
	long int sizes[7] = { 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648, 4294967296 };
	for (int j = 0; j < 7; j++) {
		double start_time = time(0);
		for (int i = 0; i < MAXCONN; i++) {
			if (fork() == 0) {
				long int skip = i * 8000000000l;
				sprintf(buffer_temp, "/bin/dd if=%s bs=%ld  iflag=skip_bytes,count_bytes skip=%ld count=8000000000 | ssh %s 'cat > temp'", argv[1], sizes[j], skip, mac_list[i]);
				printf("%s\n", buffer_temp);
				if (system(buffer_temp) == -1)
					perror("System");
				exit(0);
			}
		}
		while (wait(NULL) > 0)
			;
		printf("Time taken was %f\n", start_time - time(0));

	}

}
