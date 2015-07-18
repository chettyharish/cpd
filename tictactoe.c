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

#define MAXCONN 50
#define NUMMSG 20
#define MSGSIZE 2048
#define STLEN 100

char usr_msg[MSGSIZE];
char ret_msg[MSGSIZE];

struct mail {
	char from_username[STLEN];
	char to_username[STLEN];
	char title[STLEN];
	char text[MSGSIZE];
	long int timestamp;
	bool read_status;
};

struct user {
	int sockfd;
	int cmd_counter;
	char username[STLEN];
	char info[STLEN];
	double rating;
	int wins;
	int losses;
	int draws;
	bool quiet;
	bool playing;
	bool isloggedin;
	char [MAXCONN][STLEN];
	char sent_request[STLEN];
	struct mail mail_list[NUMMSG];
} reg_users[MAXCONN];

struct guest {
	int sockfd;
	int cmd_counter;
	char username[STLEN];
	char password[STLEN];
	bool entered_username;
	bool entered_password;
	bool trying_to_register;
} guest_users[STLEN];

struct registry {
	char username[STLEN];
	char password[STLEN];
	bool looged_in;
	int loc;
} reg_table[STLEN];

struct game {
	int gameid;
	char game_table[3][3];
	char player1[STLEN];
	char p1_color;
	char player2[STLEN];
	char p2_color;
	double start_time;
	char observers[MAXCONN][STLEN];
} game_list[MAXCONN];

void load_files() {
	for (int i = 0; i < MAXCONN; i++) {
	}

	for (int i = 0; i < MAXCONN; i++) {
		/*Default the users*/
		reg_users[i].sockfd = -1;
		reg_users[i].isloggedin = false;
	}
	for (int i = 0; i < MAXCONN; i++) {
		/*Default the games*/
	}

	for (int i = 0; i < MAXCONN; i++) {
		/*Default the gues users*/
		guest_users[i].sockfd = -1;
		strcpy(guest_users.username, "guest");
		guest_users[i].entered_username = false;
		guest_users[i].entered_password = false;
		guest_users[i].trying_to_register = false;
	}

	strcpy(reg_table[0].username, "harish");
	strcpy(reg_table[0].password, "harish");
	reg_table[0].looged_in = false;
	reg_table[0].loc = 0;
	strcpy(reg_table[1].username, "jacob");
	strcpy(reg_table[1].password, "jacob");
	reg_table[1].looged_in = false;
	reg_table[0].loc = 1;
	strcpy(reg_users[0].username, "harish");
	strcpy(reg_users[1].username, "harish");

}
void printMenu(int sockfd) {
	sprintf(ret_msg, "%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n"
			"%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n%-30s%-20s\n", "who", "# List all online users", "stats [name] ", "# Display user information", "game", "# list all current games",
			"observe <game_num>", "# Observe a game", "unobserve", "# Unobserve a game", "match <name> <b|w> [t]", "# Try to start a game", "<A|B|C><1|2|3>", "# Make a move in a game", "resign",
			"# Resign a game", "refresh", "# Refresh a game", "shout <msg>", "# shout <msg> to every one online", "tell <name> <msg>", "# tell user <name> message", "kibitz <msg> ",
			"# Comment on a game when observing", "' <msg>", "# Comment on a game", "quiet", "# Quiet mode, no broadcast messages", "nonquiet", "# Non-quiet mode", "block <id>",
			"# No more communication from <id>", "unblock <id>", "# Allow communication from <id>", "listmail", "# List the header of the mails", "readmail <msg_num>", "# Read the particular mail",
			"deletemail <msg_num>", "# Delete the particular mail", "mail <id> <title>", "# Send id a mail", "info <msg>", "# change your information to <msg>", "passwd <new>", "# change password",
			"exit", "# quit the system", "quit", "# quit the system", "help", "# print this message", "?", "# print this message", "register <name> <pwd>", "# register a new user");
//	write_bytes(sockfd, buffer_temp);
	int ret = write(sockfd, ret_msg, strlen(ret_msg));
}

void write_client_id(struct user curr_user) {
	/*Must write the client name and command number*/
	sprintf(ret_msg, "<%s>: %d", curr_user.username, curr_user.cmd_counter);
	int ret = write(curr_user.sockfd, ret_msg, strlen(ret_msg));
}

void write_wrong_syntax(struct user curr_user) {
	strcpy(ret_msg, "Syntax: Command not supported\n");
	int ret = write(curr_user.sockfd, ret_msg, strlen(ret_msg));
}

void reset_guest(struct guest guest_user) {
	close(guest_user.sockfd);
	guest_user.sockfd = -1;
	guest_user.entered_username = false;
	guest_user.entered_password = false;
	guest_user.trying_to_register = false;
}

int match_command() {
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	char *cmd;
	char *cmd = strtok(temp_cmd, "\r");
	if (strchr(cmd, ' ') != NULL) {
		/*Dont waste total_cmd in strtok*/
		cmd = strtok(temp_cmd, " ");
	} else {
		/*Has no space*/;
	}

	printf("match_command cmd = %s\n", cmd);
	fflush(0);

	if (strcmp(cmd, "who") == 0) {
		printf("Calling who\n");
		return 0;
	} else if (strcmp(cmd, "stats") == 0) {
		printf("Calling stats\n");
		return 1;
	} else if (strcmp(cmd, "game") == 0) {
		printf("Calling game\n");
		return 2;
	} else if (strcmp(cmd, "observe") == 0) {
		printf("Calling observe\n");
		return 3;
	} else if (strcmp(cmd, "unobserve") == 0) {
		printf("Calling unobserve\n");
		return 4;
	} else if (strcmp(cmd, "match") == 0) {
		printf("Calling match\n");
		return 5;
	} else if (strcmp(cmd, "resign") == 0) {
		printf("Calling resign\n");
		return 6;
	} else if (strcmp(cmd, "refresh") == 0) {
		printf("Calling refresh\n");
		return 7;
	} else if (strcmp(cmd, "shout") == 0) {
		printf("Calling shout\n");
		return 8;
	} else if (strcmp(cmd, "tell") == 0) {
		printf("Calling tell\n");
		return 9;
	} else if (strcmp(cmd, "kibitz") == 0) {
		printf("Calling kibitz\n");
		return 10;
	} else if (strcmp(cmd, "'") == 0) {
		printf("Calling '\n");
		return 11;
	} else if (strcmp(cmd, "quiet") == 0) {
		printf("Calling quiet\n");
		return 12;
	} else if (strcmp(cmd, "nonquiet") == 0) {
		printf("Calling nonquiet\n");
		return 13;
	} else if (strcmp(cmd, "block") == 0) {
		printf("Calling block\n");
		return 14;
	} else if (strcmp(cmd, "unblock") == 0) {
		printf("Calling unblock\n");
		return 15;
	} else if (strcmp(cmd, "listmail") == 0) {
		printf("Calling listmail\n");
		return 16;
	} else if (strcmp(cmd, "readmail") == 0) {
		printf("Calling readmail\n");
		return 17;
	} else if (strcmp(cmd, "deletemail") == 0) {
		printf("Calling deletemail\n");
		return 18;
	} else if (strcmp(cmd, "mail") == 0) {
		printf("Calling mail\n");
		return 19;
	} else if (strcmp(cmd, "info") == 0) {
		printf("Calling info\n");
		return 20;
	} else if (strcmp(cmd, "passwd") == 0) {
		printf("Calling passwd\n");
		return 21;
	} else if (strcmp(cmd, "exit") == 0) {
		printf("Calling exit\n");
		return 22;
	} else if (strcmp(cmd, "quit") == 0) {
		printf("Calling quit\n");
		return 23;
	} else if (strcmp(cmd, "help") == 0) {
		printf("Calling help\n");
		return 24;
	} else if (strcmp(cmd, "?") == 0) {
		printf("Calling ?\n");
		return 25;
	} else if (strcmp(cmd, "register") == 0) {
		return 26;
	} else if (strlen(cmd) == 0) {
		/*Empty line*/
		return 27;
	} else {
		/*Command not supported*/
		return 28;
	}
}

int main(int argc, char **argv) {
	/*Creating sockets from here*/
	int sockfd_server, sockfd_client;
	struct sockaddr_in saddr_server, saddr_client;
	socklen_t len = sizeof(saddr_server);
	fd_set allset, rset;
	int maxfd = 0;

	load_files();

	saddr_server.sin_addr.s_addr = INADDR_ANY;
	saddr_server.sin_family = AF_INET;
	saddr_server.sin_port = htons((short) atoi(argv[1]));

	printf("Creating socket \n");
	if ((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}

	printf("Binding socket \n");
	if (bind(sockfd_server, (struct sockaddr *) &saddr_server, sizeof(saddr_server)) < 0) {
		perror("bind");
		exit(1);
	}

	printf("Getting name socket\n");
	if (getsockname(sockfd_server, (struct sockaddr *) &saddr_server, &len) < 0) {
		perror("getsockname");
		exit(1);
	}

	printf("ip = %s, port = %d\n", inet_ntoa(saddr_server.sin_addr), saddr_server.sin_port);
	printf("Listening to socket \n");
	if (listen(sockfd_server, MAXCONN) < 0) {
		perror("listen");
		exit(1);
	}

	FD_ZERO(&allset);
	FD_SET(sockfd_server, &allset);
	maxfd = sockfd_server;

	while (1) {
		rset = allset;
		select(maxfd + 1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(sockfd_server, &rset)) {
			int i = 0;
			/* somebody tries to connect */
			if ((sockfd_client = accept(sockfd_server, (struct sockaddr *) (&saddr_client), &len)) < 0) {
				if (errno == EINTR)
					continue;
				else {
					perror("Accept");
					exit(1);
				}
			}

			printf("remote machine = %s, port = %x, %x.\n", inet_ntoa(saddr_client.sin_addr), saddr_client.sin_port, ntohs(saddr_client.sin_port));
			for (i = 0; i < MAXCONN; i++) {
				if (guest_users[i].sockfd < 0) {
					guest_users[i].sockfd = sockfd_client;
					FD_SET(guest_users[i].sockfd, &allset);
					break;
				}
			}

			if (i == MAXCONN) {
				printf("too many connections.\n");
				close(sockfd_client);
			}

			if (sockfd_client > maxfd)
				maxfd = sockfd_client;

			sprintf(ret_msg, "<username>: %d");
			int ret = write(guest_users[i].sockfd, ret_msg, strlen(ret_msg));
		} else {
			/*One of the clients has entered something*/
			for (int i = 0; i < MAXCONN; i++) {
				if (guest_users[i].sockfd < 0)
					continue;
				if (FD_ISSET(guest_users[i].sockfd, &rset)) {
					int num = read(guest_users[i].sockfd, usr_msg, MSGSIZE);
					if (num == 0) {
						/* client exists */
						printf("Guest %d has left", i);
						reset_guest(guest_users[i]);
						FD_CLR(guest_users[i].sockfd, &allset);
					} else {
						cleanup_usr_msg();
						int ret = match_command();
						if (ret != 27) {
							if (guest_users[i].entered_username == false) {
								guest_users[i].entered_username = true;
								strcpy(guest_users[i].username, usr_msg);
								sprintf(ret_msg, "<password>: %d");
								int ret = write(guest_users[i].sockfd, ret_msg, strlen(ret_msg));
							} else if (guest_users[i].entered_username == false) {
								strcpy(guest_users[i].password, usr_msg);
								if ((ret = connect_reg_user(guest_users[i])) == -1) {

								}
							}
						}

						/*Reset whatever was done*/
						memset(usr_msg, '\0', sizeof(char) * MSGSIZE);
						memset(ret_msg, '\0', sizeof(char) * MSGSIZE);
					}
				}
			}
		}
	}
}
