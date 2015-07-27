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

#define SOCKET_BLK 25000
#define START_SOCK 21000
#define ELE_PER_CLIENT 850000000
#define ELE_PER_SERVER 1200000000
#define ELE_PER_BLK 600000000
#define MAXCONN 8
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

char *exec_args[1000];
char buffer_temp[1000];

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

void write_long_chunk(int sockfd_client, char *num) {
	unsigned int size = sizeof(long int) * SOCKET_BLK;
	int rlen = 0, ret = 0;
	while (rlen < size) {
		if (unlikely((ret = write(sockfd_client, (num + rlen), size - rlen)) == -1)) {
			perror("Write long");
			exit(1);
		}
		if (unlikely(ret <= 0)) {
			exit(1);
		}
		rlen += ret;
	}
}

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("Usage ./server_king large_file_name current_ip_address machinefile\n");
		fflush(0);
		exit(1);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Get the 8 machines from machinefile*/
	FILE *file = fopen(argv[3], "r");
	char mac_list[MAXCONN][1000];
	int num_machines = 0;
	char buffer_machines[1000];
	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
		num_machines++;
	}

	if (num_machines < MAXCONN) {
		printf("Ensure that the file exists and there are exactly 8 machines which are active with 10GB memory free\n");
		exit(1);
	}
	fseek(file, 0, SEEK_SET);
	for (int i = 0; i < MAXCONN; i++) {
		/*Read the machines from machine file*/
		if (fscanf(file, "%s\n", mac_list[i]) == 0) {
			printf("Error reading machinefile = %s\n", argv[3]);
			exit(1);
		} else {
			printf("Machine i = %s\n", mac_list[i]);
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

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
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

			sprintf(buffer_temp, "/usr/bin/ssh %s ./client_king %s %d ", mac_list[i], argv[2], (START_SOCK + i));
			printf("%s\n", buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
				exit(1);
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

	long int *op_data = malloc(sizeof(long) * SOCKET_BLK);
	int pid[8];
	for (int i = 0; i < MAXCONN; i++) {
		if ((pid[i] = fork()) == 0) {
			long int start = time(0);
			FILE *in_file = fopen(argv[1], "r");
			if (!in_file) {
				printf("Input file missing\n");
				exit(1);
			}
			long int skip = i * ELE_PER_CLIENT * 8l;

			fseek(in_file, skip, SEEK_SET);

			for (long int total = 0; total < ELE_PER_CLIENT; total += SOCKET_BLK) {
				if (total % 100000000 == 0) {
					printf("Working at %ld in process %d  time = %ld\n", total, i, time(0) - start);
				}
				if (fread(op_data, sizeof(long int), SOCKET_BLK, in_file) == -1) {
					perror("fread");
					exit(1);
				}
				write_long_chunk(sfd_client[i], (char *) op_data);
			}

			printf("Process %d  Exiting at %ld\n", i, time(0) - start);
			exit(0);
		}

//		if (i == 1 || i == 3 || i == 5) {
//			int stat;
//			int stat2;
//			waitpid(pid[i - 1],&stat,0);
//			waitpid(pid[i],&stat2,0);
//			printf("%d\t%d\n", stat, stat2);
//		}

//		if (i == 3 || i == 6) {
//			int stat;
//			waitpid(pid[i - 2], &stat, 0);
//			waitpid(pid[i - 1], &stat, 0);
//			waitpid(pid[i], &stat, 0);
//			printf("%d\t%d\n", stat, stat);
//
//		}

		if (i == 3) {
			int stat;
			waitpid(pid[0], &stat, 0);
			waitpid(pid[1], &stat, 0);
			waitpid(pid[2], &stat, 0);
			waitpid(pid[3], &stat, 0);
			printf("%d\t%d\n", stat, stat);
		}
	}

	while (wait(NULL) > 0)
		;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
