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

#define ELE_PER_PC 850000000l
#define ELE_PER_BLK 425000000l
#define SOCKET_BLK ELE_PER_BLK
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

static __inline__ void read_long_chunk(int sockfd_client, char *num) {
	unsigned long int size = sizeof(long int) * SOCKET_BLK;
	long int rlen = 0;
	long int ret;
	while (rlen < size) {
		if (unlikely((ret = read(sockfd_client, (num + rlen), size - rlen)) == -1)) {
			perror("read_long");
			exit(1);
		}

		if (unlikely(ret < 0)) {
			perror("socket closed before consumption");
			exit(1);
		}
		rlen += ret;
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage ./client_king IP_server PORT_server\n");
		exit(1);
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Creating and connecting sockets here*/
	int sockfd_client;
	struct sockaddr_in saddr_client;

	if ((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		exit(1);
	}

	memset(&saddr_client, 0, sizeof(struct sockaddr_in));
	saddr_client.sin_family = AF_INET;
	saddr_client.sin_port = htons((short) atoi(argv[2]));
	saddr_client.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sockfd_client, (struct sockaddr *) &saddr_client, sizeof(saddr_client)) == -1) {
		exit(1);
	}

	long int *data = malloc(sizeof(long int) * ELE_PER_BLK);
	long int *data2 = malloc(sizeof(long int) * ELE_PER_BLK);

	for(long int count = 0 ; count < ELE_PER_BLK ; count += SOCKET_BLK){
		read_long_chunk(sockfd_client , (char *) &data[count]);
	}

	for(long int count = 0 ; count < ELE_PER_BLK ; count += SOCKET_BLK){
		read_long_chunk(sockfd_client , (char *) &data2[count]);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
