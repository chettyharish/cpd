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

int main(int argc, char **argv) {
	FILE *t = fopen("comp_and_ran", "w+");

	long int temp[3];
	for (long int i = 0; i < 3; i++) {
		temp[i] = i;
	}

	int sockfd_client;
	struct sockaddr_in saddr_client;

	if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(t, "Socket failed!\n");
		fflush(t);
		fclose(t);
		exit(1);
	}

	memset(&saddr_client, 0, sizeof(struct sockaddr_in));
	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons((short) atoi(argv[2]));
	saddr_client.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sockfd_client, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1) {
		fprintf(t, "Connect failed!\n");
		fflush(t);
		fclose(t);
		exit(1);
	}

	for (int i = 0; i < 3; i++) {
		if (write(sockfd_client, &temp[i], sizeof(&temp[i])) == -1) {
			fprintf(t, "Write failed!\n");
			fflush(t);
			fclose(t);
			exit(1);
		}
	}

	fprintf(t, "Worked correctly!\n");
	fflush(t);
	fclose(t);

	/*Cleaning the files here*/
	if (system("rm -f temp sort_client sort_client.c") == -1)
		perror("System");

}
