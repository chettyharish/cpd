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

#define NAME_LEN 1000
#define MAXCONN 7

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
	char buffer_machines[NAME_LEN];
	while (fgets(buffer_machines, sizeof buffer_machines, file)) {
		num_machines++;
	}

	if (num_machines < MAXCONN) {
		printf("Ensure that the file exists and there are exactly 7 machines which are active with 8GB memory free\n");
		exit(1);
	}
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
	/*Creating sockets from here*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in saddr_server, saddr_client;
	int sfd_server[MAXCONN];
	int sfd_client[MAXCONN];
	socklen_t len = sizeof(saddr_server);

	for (int i = 0; i < MAXCONN; i++) {
		saddr_server.sin_addr.s_addr = INADDR_ANY;
		saddr_server.sin_family = AF_INET;
		saddr_server.sin_port = htons((short) (5555 + i));

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
	/*Sending files to all the machines*/
	char buffer_temp[NAME_LEN];
	char *exec_args[NAME_LEN];

	for (int i = 0; i < MAXCONN; i++) {
		if (fork() == 0) {
			long int skip = i * 8000000000l;
//
//			sprintf(buffer_temp, "/bin/dd if=%s bs=32M  iflag=skip_bytes,count_bytes skip=%ld count=8000000000 | ssh %s 'cat > temp'", argv[1], skip, (i + 2) , mac_list[i]);
//			printf("%s\n", buffer_temp);
//			if (system(buffer_temp) == -1)
//				perror("System");

			sprintf(buffer_temp, "scp sort_client.c %s:", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			sprintf(buffer_temp, "ssh %s 'gcc -o sort_client sort_client.c "
					"-march=native -Ofast -std=c99 -lm -pedantic "
					"-pthread -fopenmp -funroll-loops'", mac_list[i]);
			printf("%s\n", buffer_temp);
			if (system(buffer_temp) == -1)
				perror("System");

			sprintf(buffer_temp, "/usr/bin/ssh %s ./sort_client %s %d", mac_list[i], argv[2], (5555 + i));
			printf("%s\n", buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
			return 0;
		}
	}
	while (wait(NULL) > 0)
		;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Accepting the connection here*/

	for (int i = 0; i < MAXCONN; i++) {
		if ((sockfd_client = accept(sfd_server[i], (struct sockaddr *) (&saddr_client), &len)) < 0) {
			perror("accept");
			exit(1);
		}

		sfd_client[i] = sockfd_client;
		printf("client[%d] is remote machine = %s, port = %x, %x.\n", i, inet_ntoa(saddr_client.sin_addr), saddr_client.sin_port, ntohs(saddr_client.sin_port));
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Time to read and merge data :)*/
	FILE *final = fopen("answer", "w+");
	long int num;
	for (int i = 0; i < MAXCONN * 3; i++) {

		if (read(sfd_client[i], (char *) &num, sizeof(num)) == -1) {
			perror("accept");
			exit(1);
		}
		fprintf(final, "%ld\n", num);
	}

	for (int i = 0; i < MAXCONN; i++) {
		sprintf(buffer_temp, "ssh %s cat comp_and_run", mac_list[i]);
		printf("%s\n", buffer_temp);
		if (system(buffer_temp) == -1)
			perror("System");
	}

	while (wait(NULL) > 0)
		;

	return 0;

}
