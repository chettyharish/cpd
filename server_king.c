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

#define SOCKET_BLK 1000
#define MAXCONN 6
#define START_SOCK 6000
#define NAME_LEN 1000

char buffer_temp[NAME_LEN];
char *exec_args[NAME_LEN];
char buffer_machines[NAME_LEN];
double start, end1, end2, end3;

void wait_all_children() {
	while (wait(NULL) > 0)
		;
}

static __inline__ void read_long(int sockfd_client, char *num) {
	unsigned int size = sizeof(long int) * SOCKET_BLK;
	int rlen = 0;
	int ret;
	while (rlen < size) {
		if ((ret = read(sockfd_client, (num + rlen), size - rlen)) == -1) {
			perror("read_long");
			exit(1);
		}

		if (ret < 0) {
			perror("socket closed before consumption");
			exit(1);
		}
		rlen += ret;
	}
}
void tokenize(char *buffer_temp, char *exec_args[]) {
	int counter = 0;
	char *token = strtok(buffer_temp, " ");
	while (token != NULL) {
		exec_args[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}
	exec_args[counter] = NULL;
}

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Usage ./sort_server large_file_name current_ip_address machinefile\n");
		exit(1);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Get the 7 machines from machinefile*/
	FILE *file = fopen(argv[3], "r");
	char mac_list[MAXCONN][1000];
	int num_machines = 0;
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*PHASE 1 STARTED*/
	/*Creating sockets from here*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in saddr_server, saddr_client;
	int sfd_server[MAXCONN];
	int sfd_client[MAXCONN];
	socklen_t len = sizeof(saddr_server);

	for (int i = 0; i < MAXCONN; i++) {
		saddr_server.sin_addr.s_addr = INADDR_ANY;
		saddr_server.sin_family = AF_INET;
		saddr_server.sin_port = htons((short) (START_SOCK + i));

		printf("Creating socket for %d\n", i);
		if ((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			exit(1);
		}

		printf("Binding socket for %d\n", i);
		if (bind(sockfd_server, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) < 0) {
			perror("bind");
			exit(1);
		}

		printf("Getting name socket for %d\n", i);
		if (getsockname(sockfd_server, (struct sockaddr *) &saddr_server, &len) < 0) {
			perror("getsockname");
			exit(1);
		}
		sfd_server[i] = sockfd_server;
		printf("Listening to socket %d\n", i);
		if (listen(sfd_server[i], 1) < 0) {
			perror("listen");
			exit(1);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	start = time(0);
//	for (int i = 1; i < 7; i++) {
//		if (fork() == 0) {
//			long int skip = i * 8000000000l;
//			sprintf(buffer_temp, "/bin/dd if=%s bs=32M  iflag=skip_bytes,count_bytes skip=%ld count=8000000000  |ssh m%d 'cat > temp%d'", argv[1], skip, i, i);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");
//			exit(0);
//		}
//	}

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
			long int skip = i * 8000000000l;
			sprintf(buffer_temp, "/bin/dd if=%s bs=32M  iflag=skip_bytes,count_bytes skip=%ld count=8000000000 | ssh %s 'cat > temp%d'", argv[1], skip, mac_list[i], START_SOCK + i);
			printf("%s\n", buffer_temp);
			fflush(0);
//			if (system(buffer_temp) == -1)
//				perror("System");

			sprintf(buffer_temp, "scp client_king.c %s:", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");
			sprintf(buffer_temp, "ssh %s gcc -o client_king client_king.c "
					"-march=native -Ofast -std=c99 -lm -pedantic "
					"-pthread -fopenmp -funroll-loops", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			exit(0);
		}
	}

	wait_all_children();

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
			for (int i = 0; i < MAXCONN; i++) {
				close(sfd_server[i]);
				close(sfd_client[i]);
			}
			close(sockfd_client);
			close(sockfd_server);
			sprintf(buffer_temp, "/usr/bin/ssh %s ./client_king %s %d ", mac_list[i], argv[2], (START_SOCK + i));
			printf("%s\n", buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}

			exit(0);
		}
	}

	/*Accepting the connection here*/
	for (int i = 0; i < MAXCONN; i++) {
		if ((sockfd_client = accept(sfd_server[i], (struct sockaddr *) (&saddr_client), &len)) < 0) {
			perror("accept");
			exit(1);
		}

		sfd_client[i] = sockfd_client;
		printf("client[%d] is remote machine = %s, port = %d.\n", i, inet_ntoa(saddr_client.sin_addr), ntohs(saddr_client.sin_port));
	}

	long int temp[SOCKET_BLK];
	long int num = 0;
	long int total = 0;
	double start = time(0);
	double end;
	printf("start = %f\n" , start);
	for (int i = 0; i < MAXCONN; i++) {
		for (long int all_count = 0; all_count < 1000000000 / SOCKET_BLK; all_count++) {
//			printf("Reading from %ld to %ld\n", all_count, all_count + SOCKET_BLK);
			read_long(sfd_client[i], (char *) &temp);
			for (long int j = 0; j < SOCKET_BLK; j++) {
				total += temp[j];
			}
		}
		end = time(0);
		printf("end = %f\n" , end);
		printf("Working at %d\t%f\n", i, end - start);
	}

	printf("Total is %ld", total);

//	printf("Time taken = %f\n", time(0) - start);
//	start = time(0);
//	for (int i = 1; i < 7; i++) {
//		if (fork() == 0) {
//			sprintf(buffer_temp, "scp m%d:temp%d .", i, i);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");
//			exit(0);
//		}
//	}
//
//	wait_all_children();
//	printf("Time taken = %f\n", time(0) - start);
//	start = time(0);
//	for (int i = 1; i < 7; i++) {
//		if (fork() == 0) {
//			sprintf(buffer_temp, "ssh m%d rm temp%d", i, i);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");
//			exit(0);
//		}
//	}
//	wait_all_children();
//	printf("Time taken = %f\n", time(0) - start);
}
