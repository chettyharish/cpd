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
	bool isfilled;
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
	int blk_list[MAXCONN];
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
} guest_users[MAXCONN];

struct registry {
	char username[STLEN];
	char password[STLEN];
	bool logged_in;
	int loc;
} reg_table[MAXCONN];

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

void set_defaults() {
	/*Setting the default values here*/
	for (int i = 0; i < MAXCONN; i++) {
		/*Default the users*/
		reg_users[i].sockfd = -1;
		reg_users[i].cmd_counter = 0;
		strcpy(reg_users[i].username, "NULL");
		strcpy(reg_users[i].info, "NULL");
		reg_users[i].rating = 0.0;
		reg_users[i].wins = 0;
		reg_users[i].losses = 0;
		reg_users[i].draws = 0;
		reg_users[i].quiet = false;
		reg_users[i].playing = false;
		reg_users[i].isloggedin = false;
		strcpy(reg_users[i].sent_request, "NULL");
		for (int j = 0; j < MAXCONN; j++) {
			reg_users[i].blk_list[j] = -1;
		}
		for (int j = 0; j < NUMMSG; j++) {
			strcpy(reg_users[i].mail_list[j].from_username, "NULL");
			strcpy(reg_users[i].mail_list[j].to_username, "NULL");
			strcpy(reg_users[i].mail_list[j].title, "NULL");
			strcpy(reg_users[i].mail_list[j].text, "NULL");
			reg_users[i].mail_list[j].timestamp = 0;
			reg_users[i].mail_list[j].read_status = false;
			reg_users[i].mail_list[j].isfilled = false;
		}

	}

	for (int i = 0; i < MAXCONN; i++) {
		/*Default the registry table*/
		for (int i = 0; i < MAXCONN; i++) {
			reg_table[i].loc = -1;
			reg_table[i].logged_in = false;
			strcpy(reg_table[i].username, "NULL");
			strcpy(reg_table[i].password, "NULL");
		}
	}

	for (int i = 0; i < MAXCONN; i++) {
		/*Default the games*/
	}

	for (int i = 0; i < MAXCONN; i++) {

		/*Default the guest users*/
		guest_users[i].sockfd = -1;
		guest_users[i].cmd_counter = 0;
		strcpy(guest_users[i].username, "guest");
		strcpy(guest_users[i].password, "NULL");
		guest_users[i].entered_username = false;
		guest_users[i].entered_password = false;
		guest_users[i].trying_to_register = false;
	}
}

void load_files() {
	set_defaults();
	/*Fake entries*/
	strcpy(reg_table[0].username, "harish");
	strcpy(reg_table[0].password, "harish");
	reg_table[0].logged_in = false;
	reg_table[0].loc = 0;
	strcpy(reg_table[1].username, "jacob");
	strcpy(reg_table[1].password, "jacob");
	reg_table[1].logged_in = false;
	reg_table[1].loc = 1;
	strcpy(reg_table[2].username, "chetty");
	strcpy(reg_table[2].password, "chetty");
	reg_table[2].logged_in = false;
	reg_table[1].loc = 2;
	strcpy(reg_table[3].username, "hello");
	strcpy(reg_table[3].password, "world");
	reg_table[3].logged_in = false;
	reg_table[3].loc = 2;
	strcpy(reg_users[0].username, "harish");
	strcpy(reg_users[1].username, "jacob");
	strcpy(reg_users[3].username, "chetty");
	strcpy(reg_users[4].username, "hello");

}

void store_files() {
	char data[2 * MSGSIZE];
	FILE *registry = fopen("ttt_registry_table", "w+");
	for (int i = 0; i < MAXCONN; i++) {
		sprintf(data, "%s,%s,%d\n", reg_table[i].username, reg_table[i].password, reg_table[i].loc);
		fprintf(registry, "%s", data);

		if (strcmp(reg_table[i].username, "NULL") != 0) {
			sprintf(data, "ttt_user_%s", reg_table[i].username);
			FILE *user_file = fopen(data, "w+");
			sprintf(data, "%s,%s,%f,%d,%d,%d,%d\n", reg_users[i].username, reg_users[i].info, reg_users[i].rating, reg_users[i].wins, reg_users[i].losses, reg_users[i].draws, reg_users[i].quiet);
			fprintf(user_file, "%s", data);
			for (int j = 0; j < MAXCONN; j++) {
				if (j != MAXCONN - 1)
					fprintf(user_file, "%d,", reg_users[i].blk_list[j]);
				else
					fprintf(user_file, "%d\n", reg_users[i].blk_list[j]);
			}
			for (int j = 0; j < NUMMSG; j++) {
				sprintf(data, "%s,%s,%s,%s,%d,%ld\n", reg_users[i].mail_list[j].from_username, reg_users[i].mail_list[j].to_username, reg_users[i].mail_list[j].title, reg_users[i].mail_list[j].text,
						reg_users[i].mail_list[j].read_status, reg_users[i].mail_list[j].timestamp);
				fprintf(user_file, "%s", data);
			}
			fclose(user_file);
		}
	}
	fclose(registry);
}

void write_return(int sockfd) {
	int ret = write(sockfd, ret_msg, strlen(ret_msg));
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

	write_return(sockfd);
}

void cleanup_usr_msg() {
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	char *cmd = strtok(temp_cmd, "\r");
	strcpy(usr_msg, cmd);
}

void write_client_id(int sockfd, char *username, int cmd_counter) {
	/*Must write the client name and command number*/
	sprintf(ret_msg, "<%s: %d>", username, cmd_counter);
	write_return(sockfd);
}

int find_user(char *username) {
	if (username == NULL)
		return -1;
	for (int i = 0; i < MAXCONN; i++) {
		if (strcmp(reg_table[i].username, username) == 0) {
			return i;
		}
	}
	return -1;
}

void write_wrong_syntax(int sockfd) {
	strcpy(ret_msg, "Syntax: Command not supported\n");
	write_return(sockfd);
}

void print_who_message(int sockfd) {
	int total = 0;
	int printed = 0;
	for (int i = 0; i < MAXCONN; i++) {
		if (reg_table[i].logged_in == true)
			total++;
	}

	sprintf(ret_msg, "Total %d user(s) online:\n", total);
	write_return(sockfd);

	for (int i = 0; i < MAXCONN; i++) {
		if (reg_table[i].logged_in == true) {
			printed++;
			if (total != 1) {
				if (printed == 1) {
					sprintf(ret_msg, "%s", reg_table[i].username);
				} else if (printed == total) {
					sprintf(ret_msg, "\t%s\n", reg_table[i].username);
				} else {
					sprintf(ret_msg, "\t%s", reg_table[i].username);
				}
			} else {
				sprintf(ret_msg, "%s\n", reg_table[i].username);
				write_return(sockfd);
				return;
			}
			write_return(sockfd);
		}
	}

}

void get_stats(int sockfd) {
	int total = 0;
	int printed = 0;
	char *username;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = strtok(temp_cmd, " ");
	username = strtok(NULL, " ");
	printf("Trying to stat username = %s\n", username);
	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(sockfd);
		return;
	} else {
		sprintf(ret_msg, "User: %s\nInfo: %s\nRating: %f\nWins: %d, Loses: %d, Draws %d\nQuiet: %s\nBlocked users: ", reg_users[ret].username,
				(strcmp(reg_users[ret].info, "NULL") == 0) ? ("<none>") : (reg_users[ret].info), reg_users[ret].rating, reg_users[ret].wins, reg_users[ret].losses, reg_users[ret].draws,
				(reg_users[ret].quiet) ? ("Yes") : ("No"));
		write_return(sockfd);
		for (int i = 0; i < MAXCONN; i++) {
			if (reg_users[ret].blk_list[i] != -1)
				total++;
		}

		if (total != 0) {
			for (int i = 0; i < MAXCONN; i++) {
				if (reg_users[ret].blk_list[i] != -1) {
					printed++;
					if (total != 1) {
						if (printed == 1) {
							sprintf(ret_msg, "%s", reg_users[reg_users[ret].blk_list[i]].username);
						} else if (printed == total) {
							sprintf(ret_msg, "\t%s\n", reg_users[reg_users[ret].blk_list[i]].username);
						} else {
							sprintf(ret_msg, "\t%s", reg_users[reg_users[ret].blk_list[i]].username);
						}
					} else {
						sprintf(ret_msg, "%s\n", reg_users[reg_users[ret].blk_list[i]].username);
						write_return(sockfd);
						return;
					}
					write_return(sockfd);
				}
			}
		} else {
			sprintf(ret_msg, "%s", "<none>\n");
			write_return(sockfd);
			return;
		}
	}
}

int connect_reg_user(char *username, char *password) {
	for (int i = 0; i < MAXCONN; i++) {
		printf("Comparing username's %s & %s\n", username, reg_table[i].username);
		if (strcmp(username, reg_table[i].username) == 0 && strcmp(reg_table[i].username, "NULL") != 0) {
			/*Username has matched*/
			if (strcmp(password, reg_table[i].password) == 0) {
				/*Password has matched*/
				return i;
			} else {
				return -1;
			}
		}
	}
	return -1;
}

void reset_guest(int gid, bool closed) {
	if (closed == true) {
		close(guest_users[gid].sockfd);
	} else {
		/*Do not close the client*/
	}
	guest_users[gid].sockfd = -1;
	guest_users[gid].cmd_counter = 0;
	strcpy(guest_users[gid].username, "guest");
	strcpy(guest_users[gid].password, "NULL");
	guest_users[gid].entered_username = false;
	guest_users[gid].entered_password = false;
	guest_users[gid].trying_to_register = false;
}

int find_free_loc() {
	for (int i = 0; i < MAXCONN; i++) {
		if (reg_table[i].loc == -1) {
			return i;
		}
	}
	return -1;
}

void register_new_user(int sockfd) {
	char *username, *password;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = strtok(temp_cmd, " ");
	username = strtok(NULL, " "); // Since first is register command
	password = strtok(NULL, " \n");
	printf("Trying to register username = %s with pass = %s\n", username, password);

	for (int i = 0; i < MAXCONN; i++) {
		if (find_user(username) != -1) {
			sprintf(ret_msg, "User %s already exists. Please try another username", username);
			write_return(sockfd);
			return;
		}
	}

	int ret = find_free_loc();
	if (ret == -1) {
		sprintf(ret_msg, "Database is full no more registration is possible\n");
		write_return(sockfd);
		return;

	}

	/*Empty spot found create new entry in both registry table and registered users*/
	/*Only the necessary ones, the default ones are correct*/
	reg_table[ret].loc = ret;
	strcpy(reg_table[ret].username, username);
	strcpy(reg_table[ret].password, password);
	strcpy(reg_users[ret].username, username);
}

int match_command() {
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	char *cmd = usr_msg;
	if (strchr(temp_cmd, ' ') != NULL) {
		/*Dont waste total_cmd in strtok*/
		cmd = strtok(temp_cmd, " ");
	} else {
		/*Has no space*/;
	}

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
		printf("Calling register\n");
		return 26;
	} else if (strlen(cmd) == 1) {
		/*Empty line*/
		printf("Empty Line\n");
		return 27;
	} else {
		/*Command not supported*/
		printf("Unknown command\n");
		return 28;
	}
}

int main(int argc, char **argv) {
	bool guest_call = false;
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
		guest_call = false;
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
				sprintf(ret_msg, "Server overloaded, please try later\n");
				write_return(sockfd_client);
				close(sockfd_client);
			}

			if (sockfd_client > maxfd)
				maxfd = sockfd_client;

			sprintf(ret_msg, "<username>:");
			write_return(guest_users[i].sockfd);
		} else {
			/*One of the guests has entered something*/
			for (int i = 0; i < MAXCONN; i++) {
				/*Loop for testing out guests*/
				if (guest_users[i].sockfd < 0)
					continue;
				if (FD_ISSET(guest_users[i].sockfd, &rset)) {
					int num = read(guest_users[i].sockfd, usr_msg, MSGSIZE);
					if (num == 0) {
						/* client has left */
						printf("Guest %d has left", i);
						FD_CLR(guest_users[i].sockfd, &allset);
						reset_guest(i, true);
					} else {
						cleanup_usr_msg();
						int ret = match_command();

						switch (ret) {
						case 27:
							if (guest_users[i].entered_username == false) {
								/*Going to register mode*/
								guest_users[i].trying_to_register = true;
							}
							write_client_id(guest_users[i].sockfd, guest_users[i].username, guest_users[i].cmd_counter);
							guest_users[i].cmd_counter++;
							break;
						case 28:
							if (guest_users[i].trying_to_register == false) {
								if (guest_users[i].entered_username == false) {
									/*Guest has entered some username*/
									guest_users[i].entered_username = true;
									strcpy(guest_users[i].username, usr_msg);
									sprintf(ret_msg, "<password>:");
									write_return(guest_users[i].sockfd);
								} else if (guest_users[i].entered_username == true && guest_users[i].entered_password == false) {
									/*Guest has entered password*/
									strcpy(guest_users[i].password, usr_msg);
									int userid;
									if ((userid = connect_reg_user(guest_users[i].username, guest_users[i].password)) == -1) {
										/*Username match failed*/
										printf("Guest %d has left\n", i);
										sprintf(ret_msg, "Login failed!!\nThank you for using Online Tic-tac-toe Server.\nSee you next time.\n");
										write_return(guest_users[i].sockfd);
										FD_CLR(guest_users[i].sockfd, &allset);
										reset_guest(i, true);
									} else {
										/*There was a username match, so copy to client_list and release guest*/
										printf("Guest %d is now client %d\n", i, userid);
										reg_table[userid].logged_in = true;
										reg_users[userid].sockfd = guest_users[i].sockfd;
										reg_users[userid].isloggedin = true;
										reset_guest(i, false);
										write_client_id(reg_users[userid].sockfd, reg_users[userid].username, reg_users[userid].cmd_counter);
										printf("User %s has logged in\n", reg_users[userid].username);
									}
								}
							} else {
								/*Guest is trying to register and wrote a stupid command*/
								write_wrong_syntax(guest_users[i].sockfd);
								write_client_id(guest_users[i].sockfd, guest_users[i].username, guest_users[i].cmd_counter);
								guest_users[i].cmd_counter++;
							}
							break;

						case 22:
						case 23:
							/*Exiting via quit or exit*/
							printf("Guest %d has left\n", i);
							sprintf(ret_msg, "Thank you for using Online Tic-tac-toe Server.\nSee you next time.\n");
							write_return(guest_users[i].sockfd);
							FD_CLR(guest_users[i].sockfd, &allset);
							reset_guest(i, true);
							break;
						case 24:
							/*Calling for help via help or ?*/
						case 25:
							store_files();
							printMenu(guest_users[i].sockfd);
							guest_users[i].cmd_counter++;
							write_client_id(guest_users[i].sockfd, guest_users[i].username, guest_users[i].cmd_counter);
							break;
						case 26:
							register_new_user(guest_users[i].sockfd);
							write_client_id(guest_users[i].sockfd, guest_users[i].username, guest_users[i].cmd_counter);
							break;
						default:
							/*Guest is trying to register and trying to run commands*/
							sprintf(ret_msg, "Guests cannot use commands, please register first\n");
							write_return(guest_users[i].sockfd);
							write_client_id(guest_users[i].sockfd, guest_users[i].username, guest_users[i].cmd_counter);
							guest_users[i].cmd_counter++;
							break;
						}

						guest_call = true;
					}
				}
			}

			/*One of the clients has entered something*/
			/*Need to skip guest_call when switching from guest to client after registration*/
			if (guest_call == false) {
				for (int i = 0; i < MAXCONN; i++) {
					/*Loop for testing out guests*/
					if (reg_users[i].sockfd < 0)
						continue;
					if (FD_ISSET(reg_users[i].sockfd, &rset)) {
						int num = read(reg_users[i].sockfd, usr_msg, MSGSIZE);
						if (num == 0) {
							/* client has left */
							printf("Client %d has left", i);
							reg_table[i].logged_in = false;
							reg_users[i].isloggedin = false;
							reg_users[i].isloggedin = false;
							FD_CLR(reg_users[i].sockfd, &allset);
							reg_users[i].sockfd = -1;
						} else {
							cleanup_usr_msg();
							int ret = match_command();
							switch (ret) {
							case 0:
								/*who call*/
								print_who_message(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 1:
								/*stats call*/
								get_stats(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 12:
								/*quiet call*/
								reg_users[i].quiet = true;
								sprintf(ret_msg, "Quiet mode activated\n");
								write_return(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 13:
								/*unquiet call*/
								reg_users[i].quiet = false;
								sprintf(ret_msg, "Quiet mode deactivated\n");
								write_return(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 14:
								/*Block*/
								block_cmd(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 15:
								/*Unblock*/
								unblock_cmd(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 22:
							case 23:
								/*Exiting via quit or exit*/
								printf("Client %d has left\n", i);
								sprintf(ret_msg, "Thank you for using Online Tic-tac-toe Server.\nSee you next time.\n");
								write_return(reg_users[i].sockfd);
								FD_CLR(reg_users[i].sockfd, &allset);
								reg_table[i].logged_in = false;
								reg_users[i].isloggedin = false;
								reg_users[i].isloggedin = false;
								close(reg_users[i].sockfd);
								reg_users[i].sockfd = -1;
								break;
							case 24:
								/*Calling for help via help or ?*/
							case 25:
								printMenu(reg_users[i].sockfd);
								reg_users[i].cmd_counter++;
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 26:
								sprintf(ret_msg, "Only guest users can register.\n");
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 27:
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 28:
								write_wrong_syntax(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							}
							reg_users[i].cmd_counter++;
						}
					} // user switch ends
				} // user for ends
			}
		}

		/*Reset whatever was done*/
		memset(usr_msg, '\0', sizeof(char) * MSGSIZE);
		memset(ret_msg, '\0', sizeof(char) * MSGSIZE);
	}
}
