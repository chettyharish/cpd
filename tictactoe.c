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
#include <string.h>

#define brutal 2
#define MAXCONN 50
#define NUMMSG 20
#define MSGSIZE 4096
#define STLEN 100
char usr_msg[MSGSIZE];
char ret_msg[MSGSIZE];

struct mail {
	char from_username[STLEN];
	char to_username[STLEN];
	char title[STLEN];
	char text[MSGSIZE];
	char timestamp[STLEN];
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
	bool isloggedin;
	int blk_list[MAXCONN];
	/*Mail temp backup and mailing stuff*/
	struct mail mail_list[NUMMSG];
	char message_body[MSGSIZE];
	bool sending_mail;
	int temp_sending_to;
	char temp_title[STLEN];
	char temp_timestamp[STLEN];
	/*To Handle games*/
	bool trying_to_match;
	int trying_to_match_player;
	int trying_to_match_color;
	int trying_to_match_time;
	bool playing;
	bool isobserving;
	int gameid;
	int observeids[MAXCONN];
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
	bool in_use;
	int num_moves;
	int observers[MAXCONN];
	int player1;
	int player2;
	bool p1_move;
	bool p2_move;
	int game_table[3][3];
	int p1_color;
	int p2_color;
	size_t p1_start;
	size_t p2_start;
	double p1_time;
	double p2_time;
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
		strcpy(reg_users[i].message_body, "NULL");
		for (int j = 0; j < MAXCONN; j++) {
			reg_users[i].blk_list[j] = -1;
			reg_users[i].observeids[j] = -1;
		}
		for (int j = 0; j < NUMMSG; j++) {
			strcpy(reg_users[i].mail_list[j].from_username, "NULL");
			strcpy(reg_users[i].mail_list[j].to_username, "NULL");
			strcpy(reg_users[i].mail_list[j].title, "NULL");
			strcpy(reg_users[i].mail_list[j].text, "NULL");
			strcpy(reg_users[i].mail_list[j].timestamp, "NULL");
			reg_users[i].mail_list[j].read_status = false;
			reg_users[i].mail_list[j].isfilled = false;
		}

	}

	/*Default the registry table*/
	for (int i = 0; i < MAXCONN; i++) {
		reg_table[i].loc = -1;
		reg_table[i].logged_in = false;
		strcpy(reg_table[i].username, "NULL");
		strcpy(reg_table[i].password, "NULL");
	}

	for (int i = 0; i < MAXCONN; i++) {
		/*Default the games*/
		game_list[i].in_use = false;
		game_list[i].num_moves = 0;
		game_list[i].player1 = -1;
		game_list[i].player2 = -1;
		game_list[i].p1_move = false;
		game_list[i].p2_move = false;
		game_list[i].p1_color = -1;
		game_list[i].p2_color = -1;
		game_list[i].p1_time = 0;
		game_list[i].p2_time = 0;
		game_list[i].game_table[0][0] = -1;
		game_list[i].game_table[0][1] = -1;
		game_list[i].game_table[0][2] = -1;
		game_list[i].game_table[1][0] = -1;
		game_list[i].game_table[1][1] = -1;
		game_list[i].game_table[1][2] = -1;
		game_list[i].game_table[2][0] = -1;
		game_list[i].game_table[2][1] = -1;
		game_list[i].game_table[2][2] = -1;
		for (int j = 0; j < MAXCONN; j++) {
			game_list[i].observers[j] = -1;
		}
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

void remove_endlines(char *text) {
	for (int i = 0; i < strlen(text); i++) {
		if (text[i] == '\n')
			text[i] = '|';
	}
}

void add_endlines(char *text) {
	for (int i = 0; i < strlen(text); i++) {
		if (text[i] == '|')
			text[i] = '\n';
	}
}

void load_files() {
	set_defaults();
	/*Fake entries*/
	strcpy(reg_table[0].username, "a");
	strcpy(reg_table[0].password, "a");
	reg_table[0].logged_in = false;
	reg_table[0].loc = 0;
	strcpy(reg_table[1].username, "b");
	strcpy(reg_table[1].password, "b");
	reg_table[1].logged_in = false;
	reg_table[1].loc = 1;
	strcpy(reg_table[2].username, "c");
	strcpy(reg_table[2].password, "c");
	reg_table[2].logged_in = false;
	reg_table[2].loc = 2;
	strcpy(reg_table[3].username, "d");
	strcpy(reg_table[3].password, "d");
	reg_table[3].logged_in = false;
	reg_table[3].loc = 3;
	strcpy(reg_table[4].username, "e");
	strcpy(reg_table[4].password, "e");
	reg_table[4].logged_in = false;
	reg_table[4].loc = 4;
	strcpy(reg_table[5].username, "f");
	strcpy(reg_table[5].password, "f");
	reg_table[5].logged_in = false;
	reg_table[5].loc = 5;
	strcpy(reg_users[0].username, "a");
	strcpy(reg_users[1].username, "b");
	strcpy(reg_users[2].username, "c");
	strcpy(reg_users[3].username, "d");
	strcpy(reg_users[4].username, "e");
	strcpy(reg_users[5].username, "f");
}

void load_db() {
	set_defaults();
	char data[2 * MSGSIZE];
	char temp_msg[2 * MSGSIZE];
	FILE *registry = fopen("ttt_registry_table", "r");

	if (registry == NULL) {
		/*First time running the code, there are no users*/
		return;
	}

	char *username, *password, *loc, *rest;
	for (int i = 0; i < MAXCONN; i++) {
		if (fgets(data, 2 * MSGSIZE, registry) == NULL) {
			perror("fgets");
		}
		username = __strtok_r(data, "^", &rest);
		password = __strtok_r(NULL, "^", &rest);
		loc = __strtok_r(NULL, "\n", &rest);
		if (strcmp(username, "NULL") == 0)
			break;

		strcpy(reg_table[i].username, username);
		strcpy(reg_table[i].password, password);
		reg_table[i].loc = atoi(loc);
		printf("Read username = %s , password = %s , loc = %d\n", username, password, atoi(loc));
	}

	char *username2, *info, *rating, *wins, *losses, *draws, *quiet;
	for (int i = 0; i < MAXCONN; i++) {
		if (strcmp(reg_table[i].username, "NULL") == 0)
			break;
		sprintf(data, "ttt_user_%s", reg_table[i].username);
		FILE *user_file = fopen(data, "r");
		if (fgets(data, 2 * MSGSIZE, user_file) == NULL) {
			perror("fgets");
		}
		username2 = __strtok_r(data, "^", &rest);
		info = __strtok_r(NULL, "^", &rest);
		rating = __strtok_r(NULL, "^", &rest);
		wins = __strtok_r(NULL, "^", &rest);
		losses = __strtok_r(NULL, "^", &rest);
		draws = __strtok_r(NULL, "^", &rest);
		quiet = __strtok_r(NULL, "^", &rest);
//		printf("u2 = %s , i = %s , r = %f , w = %d, l = %d , d = %d , q = %d\n", username2, info, atof(rating), atoi(wins), atoi(losses), atoi(draws), atoi(quiet));

		strcpy(reg_users[i].username, username2);
		strcpy(reg_users[i].info, info);
		reg_users[i].rating = atof(rating);
		reg_users[i].wins = atoi(wins);
		reg_users[i].losses = atoi(losses);
		reg_users[i].draws = atoi(draws);
		reg_users[i].quiet = atoi(quiet);

		for (int j = 0; j < NUMMSG; j++) {
			char *from, *to, *title, *text, *read_status, *timestamp, *isfilled;
			if (fgets(data, 2 * MSGSIZE, user_file) == NULL) {
				perror("fgets");
			}
			from = __strtok_r(data, "^", &rest);
			to = __strtok_r(NULL, "^", &rest);
			title = __strtok_r(NULL, "^", &rest);
			text = __strtok_r(NULL, "^", &rest);
			read_status = __strtok_r(NULL, "^", &rest);
			timestamp = __strtok_r(NULL, "^", &rest);
			isfilled = __strtok_r(NULL, "\n", &rest);
			add_endlines(text);
//			printf("f = %s, t = %s , t = %s  , rs = %d , ts = %s\n", from, to, title, atoi(read_status), timestamp);
//			printf("text : %s", text);
			strcpy(reg_users[i].mail_list[j].from_username, from);
			strcpy(reg_users[i].mail_list[j].to_username, to);
			strcpy(reg_users[i].mail_list[j].title, title);
			strcpy(reg_users[i].mail_list[j].text, text);
			reg_users[i].mail_list[j].read_status = atoi(read_status);
			strcpy(reg_users[i].mail_list[j].timestamp, timestamp);
			reg_users[i].mail_list[j].isfilled = atoi(isfilled);
		}

		if (fgets(data, 2 * MSGSIZE, user_file) == NULL) {
			perror("fgets");
		}
		char *blk;
		blk = __strtok_r(data, "^", &rest);
		for (int j = 0; j < MAXCONN; j++) {
//			printf("j = %d blk = %d\n",j, atoi(blk));
			reg_users[i].blk_list[j] = atoi(blk);
			blk = __strtok_r(NULL, "^", &rest);
		}
		fclose(user_file);
	}
}

void store_db() {
	char data[2 * MSGSIZE];
	char temp_msg[2 * MSGSIZE];
	FILE *registry = fopen("ttt_registry_table", "w+");
	for (int i = 0; i < MAXCONN; i++) {
		sprintf(data, "%s^%s^%d\n", reg_table[i].username, reg_table[i].password, reg_table[i].loc);
		fprintf(registry, "%s", data);
		if (strcmp(reg_table[i].username, "NULL") != 0) {
			sprintf(data, "ttt_user_%s", reg_table[i].username);
			FILE *user_file = fopen(data, "w+");
			sprintf(data, "%s^%s^%f^%d^%d^%d^%d\n", reg_users[i].username, reg_users[i].info, reg_users[i].rating, reg_users[i].wins, reg_users[i].losses, reg_users[i].draws, reg_users[i].quiet);
			fprintf(user_file, "%s", data);
			for (int j = 0; j < NUMMSG; j++) {
				strcpy(temp_msg, reg_users[i].mail_list[j].text);
				remove_endlines(temp_msg);
				sprintf(data, "%s^%s^%s^%s^%d^%s^%d\n", reg_users[i].mail_list[j].from_username, reg_users[i].mail_list[j].to_username, reg_users[i].mail_list[j].title, temp_msg,
						reg_users[i].mail_list[j].read_status, reg_users[i].mail_list[j].timestamp, reg_users[i].mail_list[j].isfilled);
				fprintf(user_file, "%s", data);
			}
			for (int j = 0; j < MAXCONN; j++) {
				if (j < MAXCONN - 1)
					fprintf(user_file, "%d^", reg_users[i].blk_list[j]);
				else
					fprintf(user_file, "%d\n", reg_users[i].blk_list[j]);
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
	char id_msg[STLEN];
	sprintf(id_msg, "<%s: %d>", username, cmd_counter);
	int ret = write(sockfd, id_msg, strlen(id_msg));
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
	//write_return(sockfd);

	for (int i = 0; i < MAXCONN; i++) {
		if (reg_table[i].logged_in == true) {
			printed++;
			if (total != 1) {
				if (printed == 1) {
					sprintf(ret_msg + strlen(ret_msg), "%s", reg_table[i].username);
				} else if (printed == total) {
					sprintf(ret_msg + strlen(ret_msg), "\t%s\n", reg_table[i].username);
				} else {
					sprintf(ret_msg + strlen(ret_msg), "\t%s", reg_table[i].username);
				}
			} else {
				sprintf(ret_msg + strlen(ret_msg), "%s\n", reg_table[i].username);
				write_return(sockfd);
				return;
			}
			//write_return(sockfd);
		}
	}
	write_return(sockfd);
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
		sprintf(ret_msg, "!STATS!\nUser: %s\nInfo: %s\nRating: %f\nWins: %d, Loses: %d, Draws %d\nQuiet: %s\nBlocked users: ", reg_users[ret].username,
				(strcmp(reg_users[ret].info, "NULL") == 0) ? ("<none>") : (reg_users[ret].info), reg_users[ret].rating, reg_users[ret].wins, reg_users[ret].losses, reg_users[ret].draws,
				(reg_users[ret].quiet) ? ("Yes") : ("No"));

//		write_return(sockfd);

		for (int i = 0; i < MAXCONN; i++) {
			if (reg_users[ret].blk_list[i] != -1)
				total++;
		}

		if (total == 0) {
//			sprintf(ret_msg, "%s", "<none>");
//			write_return(sockfd);
			sprintf(ret_msg + strlen(ret_msg), "%s\n", "<none>");
		} else {
			for (int i = 0; i < MAXCONN; i++) {
				if (reg_users[ret].blk_list[i] != -1) {
					printed++;
					if (total != 1) {
						if (printed == 1) {
							sprintf(ret_msg + strlen(ret_msg), "%s", reg_users[reg_users[ret].blk_list[i]].username);
						} else if (printed == total) {
							sprintf(ret_msg + strlen(ret_msg), "\t%s\n", reg_users[reg_users[ret].blk_list[i]].username);
						} else {
							sprintf(ret_msg + strlen(ret_msg), "\t%s", reg_users[reg_users[ret].blk_list[i]].username);
						}
					} else {
						sprintf(ret_msg + strlen(ret_msg), "%s\n", reg_users[reg_users[ret].blk_list[i]].username);
//						sprintf(ret_msg, "%s\n", reg_users[reg_users[ret].blk_list[i]].username);
//						write_return(sockfd);
						return;
					}
//					write_return(sockfd);
				}
			}

		}
		write_return(sockfd);
	}
}

void block_cmd(int uid) {
	int total = 0;
	int printed = 0;
	char *username;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = strtok(temp_cmd, " ");
	username = strtok(NULL, " ");
	printf("Trying to block username = %s\n", username);
	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(reg_users[uid].sockfd);
		return;
	} else {
		/*Test if user was already blocked*/
		for (int i = 0; i < MAXCONN; i++) {
			if (reg_users[uid].blk_list[i] == ret) {
				sprintf(ret_msg, "User <%s> is already blocked\n", username);
				write_return(reg_users[uid].sockfd);
				return;
			}

		}
		/*Find a spot in the array to block*/
		for (int i = 0; i < MAXCONN; i++) {
			if (reg_users[uid].blk_list[i] == -1) {
				reg_users[uid].blk_list[i] = ret;
				sprintf(ret_msg, "User <%s> has been blocked\n", username);
				write_return(reg_users[uid].sockfd);
				return;
			}
		}
	}

	sprintf(ret_msg, "Your block_list is full\n");
	write_return(reg_users[uid].sockfd);
}

void unblock_cmd(int uid) {
	int total = 0;
	int printed = 0;
	char *username;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = strtok(temp_cmd, " ");
	username = strtok(NULL, " ");
	printf("Trying to unblock username = %s\n", username);
	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(reg_users[uid].sockfd);
		return;
	} else {
		/*Find a spot in the array to block*/
		for (int i = 0; i < MAXCONN; i++) {
			if (reg_users[uid].blk_list[i] == ret) {
				printf("User was found at location %d,%d\n", i, ret);
				reg_users[uid].blk_list[i] = -1;
				sprintf(ret_msg, "User <%s> has been unblocked\n", username);
				write_return(reg_users[uid].sockfd);
				return;
			}
		}
	}

	sprintf(ret_msg, "User <%s> was never blocked\n", username);
	write_return(reg_users[uid].sockfd);
}

void set_info(int uid) {
	printf("Changing info\n");
	char *cmd, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	cmd = __strtok_r(temp_cmd, " ", &msg);

	if (msg == NULL) {
		sprintf(ret_msg, "Please enter a new info message\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	strcpy(reg_users[uid].info, msg);
}

void set_passwd(int uid) {
	printf("Changing password\n");
	char *passwd;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	passwd = strtok(temp_cmd, " ");
	passwd = strtok(NULL, " ");
	if (passwd == NULL) {
		sprintf(ret_msg, "Please enter a new password\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	strcpy(reg_table[uid].password, passwd);
}

bool check_blocked(int uid, int tid) {
	for (int i = 0; i < MAXCONN; i++) {
		if (reg_users[uid].blk_list[i] == tid)
			return true;
	}
	return false;
}

void shout_msg(int uid) {
	char *cmd, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	cmd = __strtok_r(temp_cmd, " ", &msg);

	if (msg == NULL) {
		sprintf(ret_msg, "Please enter a message\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	for (int i = 0; i < MAXCONN; i++) {
		if (reg_users[i].isloggedin == true && check_blocked(i, uid) == false && reg_users[i].quiet == false) {
			if (uid != i) {
				sprintf(ret_msg, "\n!shout! *%s* : %s\n", reg_users[uid].username, msg);
				write_return(reg_users[i].sockfd);
				write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
			} else {
				sprintf(ret_msg, "!shout! *%s* : %s\n", reg_users[uid].username, msg);
				write_return(reg_users[i].sockfd);

			}
		}
	}
}

void tell_msg(int uid) {
	char *username, *cmd, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = __strtok_r(temp_cmd, " ", &msg);
	username = __strtok_r(NULL, " ", &msg);

	if (msg == NULL) {
		sprintf(ret_msg, "Please enter a message\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	printf("Telling username = %s\t msg = %s\n", username, msg);
	fflush(stdout);

	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	if (reg_users[ret].isloggedin == false) {
		sprintf(ret_msg, "User %s is not online.\n", username);
		write_return(reg_users[uid].sockfd);
		return;
	}

	if (check_blocked(ret, uid) == true) {
		sprintf(ret_msg, "You cannot talk to %s. You are blocked\n", username);
		write_return(reg_users[uid].sockfd);
		return;
	}

	/*Send the message*/
	sprintf(ret_msg, "\n!private! %s : %s\n", reg_users[uid].username, msg);
	write_return(reg_users[ret].sockfd);
	write_client_id(reg_users[ret].sockfd, reg_users[ret].username, reg_users[ret].cmd_counter);
	return;

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

void reset_client(int uid) {
	close(reg_users[uid].sockfd);
	reg_users[uid].sockfd = -1;
	reg_users[uid].cmd_counter = 0;
	reg_users[uid].isloggedin = false;
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void list_games(int sockfd) {
	int total = 0;
	for (int i = 0; i < MAXCONN; i++) {
		if (game_list[i].in_use == true) {
			total++;
		}
	}

	sprintf(ret_msg, "Total %d game(s):\n", total);
	//write_return(sockfd);
	total = 0;
	for (int i = 0; i < MAXCONN; i++) {
		if (game_list[i].in_use == true) {
			sprintf(ret_msg + strlen(ret_msg), "Game %d(%d): %s .vs. %s, %d moves\n", total, i, reg_users[game_list[i].player1].username, reg_users[game_list[i].player2].username,
					game_list[i].num_moves);
			//write_return(sockfd);
			total++;
		}
	}
	write_return(sockfd);
}

void print_game_table(int gid, bool to_all, int uid) {
	sprintf(ret_msg, "\nBlack:%10s\tWhite:%10s\nTime:%5d seconds\t%5d seconds\n   1  2  3\nA  %c  %c  %c\nB  %c  %c  %c\nC  %c  %c  %c\n",
			((game_list[gid].p1_color == 0) ? (reg_users[game_list[gid].player1].username) : (reg_users[game_list[gid].player2].username)),
			((game_list[gid].p1_color == 1) ? (reg_users[game_list[gid].player1].username) : (reg_users[game_list[gid].player2].username)), (int) game_list[gid].p1_time, (int) game_list[gid].p2_time,
			(game_list[gid].game_table[0][0] == -1) ? ('.') : ((game_list[gid].game_table[0][0] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[0][1] == -1) ? ('.') : ((game_list[gid].game_table[0][1] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[0][2] == -1) ? ('.') : ((game_list[gid].game_table[0][2] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[1][0] == -1) ? ('.') : ((game_list[gid].game_table[1][0] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[1][1] == -1) ? ('.') : ((game_list[gid].game_table[1][1] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[1][2] == -1) ? ('.') : ((game_list[gid].game_table[1][2] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[2][0] == -1) ? ('.') : ((game_list[gid].game_table[2][0] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[2][1] == -1) ? ('.') : ((game_list[gid].game_table[2][1] == 0) ? ('#') : ('O')),
			(game_list[gid].game_table[2][2] == -1) ? ('.') : ((game_list[gid].game_table[2][2] == 0) ? ('#') : ('O')));

	printf("Table = \n");
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			printf("%d   ", game_list[gid].game_table[i][j]);
		}
		printf("\n");
	}
	if (to_all == true) {
		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[gid].observers[i] != -1) {
				/*Print stuff to observers*/
				write_return(reg_users[game_list[gid].observers[i]].sockfd);
				write_client_id(reg_users[game_list[gid].observers[i]].sockfd, reg_users[game_list[gid].observers[i]].username, reg_users[game_list[gid].observers[i]].cmd_counter);
			}
		}
		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
	} else {
		/*a refresh call or a new observer*/
		write_return(reg_users[uid].sockfd);
	}
	return;
}

int test_match_response(int player1, int player2) {
	/*Detect difference between request or response matches*/
	/*0 if resquest
	 * 1 if mismatch in request
	 * 2 if both want same game*/

	if (reg_users[player1].trying_to_match == true && reg_users[player1].trying_to_match_player == player2) {
		/*It maybe a response message ,so check for compatible colors and times*/
		if ((reg_users[player1].trying_to_match_color != reg_users[player2].trying_to_match_color) && (reg_users[player1].trying_to_match_time == reg_users[player2].trying_to_match_time)) {
			/*Both want same stuff so accept*/
			return 2;
		} else {
			/*Both want a match, but with different specs*/
			return 1;
		}
	}

	/*Its a new request*/
	return 0;
}

void setup_match(int uid) {
	char *username, *color, *time_num, *rest;
	bool color_selected = false;
	int game_time = 600;
	int color_player = 0;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = __strtok_r(temp_cmd, " ", &rest);
	username = __strtok_r(NULL, " ", &rest);

	if (reg_users[uid].playing == true) {
		sprintf(ret_msg, "You are already playing a game\n");
		write_return(reg_users[uid].sockfd);
		return;

	}
	if (username == NULL) {
		sprintf(ret_msg, "Please supply a correct username\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	if (ret == uid) {
		/*User not found*/
		sprintf(ret_msg, "You cannot play against yourself\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	if (reg_users[ret].isloggedin == false) {
		sprintf(ret_msg, "User %s is not online.\n", username);
		write_return(reg_users[uid].sockfd);
		return;
	}

	if (reg_users[ret].playing == true) {
		sprintf(ret_msg, "User %s is already playing a game.\n", username);
		write_return(reg_users[uid].sockfd);
		return;
	}
	if (check_blocked(ret, uid) == true) {
		sprintf(ret_msg, "You cannot match to %s. You are blocked\n", username);
		write_return(reg_users[uid].sockfd);
		return;
	}

	color = __strtok_r(NULL, " ", &rest);
	if (color == NULL) {
		printf("No color selected\n");
		color_player = 0;
		color_selected = true;
	} else if (strcmp(color, "b") == 0) {
		printf("Black color selected\n");
		color_player = 0;
		color_selected = true;
	} else if (strcmp(color, "w") == 0) {
		printf("White color selected\n");
		color_player = 1;
		color_selected = true;
	} else {
		/*Wrong color*/
		sprintf(ret_msg, "Wrong color, please use b/w or nothing\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	if (color_selected == true) {
		/*Only try to read time if color is selected*/
		time_num = __strtok_r(NULL, " ", &rest);
		if (time_num != NULL) {
			game_time = atoi(time_num);
		}
	}

	reg_users[uid].trying_to_match = true;
	reg_users[uid].trying_to_match_player = ret;
	reg_users[uid].trying_to_match_color = color_player;
	reg_users[uid].trying_to_match_time = game_time;

	int status = test_match_response(ret, uid);
	if (status == 2) {
		/*Is a match response and the other player has accepted*/
		/*Setup and start the game here*/
		printf("Game: %s %s color = %d time = %d\n", reg_users[uid].username, reg_users[ret].username, color_player, game_time);
		sprintf(ret_msg, "\n Match started\n");
		write_return(reg_users[ret].sockfd);
		write_return(reg_users[uid].sockfd);
		/*Start game here*/
		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[i].in_use == false) {
				/*Found a open game, copy the stuff*/
				game_list[i].in_use = true;
				game_list[i].player1 = ret;
				game_list[i].player2 = uid;
				reg_users[uid].playing = true;
				reg_users[ret].playing = true;
				if (reg_users[ret].trying_to_match_color == 0) {
					game_list[i].p1_start = time(0);
					game_list[i].p1_move = true;
					game_list[i].p1_color = 0;
					game_list[i].p2_color = 1;
				} else {
					game_list[i].p2_start = time(0);
					game_list[i].p2_move = true;
					game_list[i].p1_color = 1;
					game_list[i].p2_color = 0;
				}

				/*Timing stuff*/
				game_list[i].p1_time = game_time;
				game_list[i].p2_time = game_time;
				printf("request = %d , response = %d\n", ret, uid);
				print_game_table(i, true, -1);
				write_client_id(reg_users[ret].sockfd, reg_users[ret].username, reg_users[ret].cmd_counter);

				/*Delete the temp stuff*/
				reg_users[uid].trying_to_match = false;
				reg_users[uid].trying_to_match_color = -1;
				reg_users[uid].trying_to_match_time = 0;
				reg_users[uid].trying_to_match_player = -1;

				reg_users[ret].trying_to_match = false;
				reg_users[ret].trying_to_match_color = -1;
				reg_users[ret].trying_to_match_time = 0;
				reg_users[uid].trying_to_match_player = -1;
				return;
			}
		}

	} else if (status == 1) {
		/*Is a match response with mismatch*/
		printf("Game: %s %s color = %d time = %d\n", reg_users[uid].username, reg_users[ret].username, color_player, game_time);
		sprintf(ret_msg, "\n%s wants <match %s %s %d>; %s wants <match %s %s %d>\n", reg_users[uid].username, reg_users[ret].username, ((reg_users[uid].trying_to_match_color == 0) ? "b" : "w"),
				reg_users[uid].trying_to_match_time, reg_users[ret].username, reg_users[uid].username, ((reg_users[ret].trying_to_match_color == 0) ? "b" : "w"), reg_users[ret].trying_to_match_time);
		write_return(reg_users[ret].sockfd);
		write_client_id(reg_users[ret].sockfd, reg_users[ret].username, reg_users[ret].cmd_counter);
		sprintf(ret_msg, "%s wants <match %s %s %d>; %s wants <match %s %s %d>\n", reg_users[uid].username, reg_users[ret].username, ((reg_users[uid].trying_to_match_color == 0) ? "b" : "w"),
				reg_users[uid].trying_to_match_time, reg_users[ret].username, reg_users[uid].username, ((reg_users[ret].trying_to_match_color == 0) ? "b" : "w"), reg_users[ret].trying_to_match_time);
		write_return(reg_users[uid].sockfd);
		write_client_id(reg_users[uid].sockfd, reg_users[uid].username, reg_users[uid].cmd_counter);
	} else if (status == 0) {
		/*Is a match request, so notify the other player*/
		printf("Game: %s %s color = %d time = %d\n", reg_users[uid].username, reg_users[ret].username, color_player, game_time);
		sprintf(ret_msg, "\n%s invites your for a game <match %s %s %d >\n", reg_users[uid].username, reg_users[uid].username, ((reg_users[uid].trying_to_match_color == 0) ? "w" : "b"),
				reg_users[uid].trying_to_match_time);
		write_return(reg_users[ret].sockfd);
		write_client_id(reg_users[ret].sockfd, reg_users[ret].username, reg_users[ret].cmd_counter);
		return;

	}

}

void observe_game(int uid) {
	char *gid_str, *rest;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	gid_str = __strtok_r(temp_cmd, " ", &rest);
	gid_str = __strtok_r(NULL, " ", &rest);

	if (gid_str == NULL) {
		sprintf(ret_msg, "Please enter a game id\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	int game_id = atoi(gid_str);
	if (game_list[game_id].in_use == false) {
		sprintf(ret_msg, "Please check the game id\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	int ob_loc = -1;
	for (int i = 0; i < MAXCONN; i++) {
		if (reg_users[uid].observeids[i] == -1) {
			reg_users[uid].observeids[i] = game_id;
			ob_loc = game_id;
			break;
		}
	}

	if (ob_loc == -1) {
		sprintf(ret_msg, "You cannot observe more games\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	for (int i = 0; i < MAXCONN; i++) {
		if (game_list[game_id].observers[i] == -1) {
			/*Empty spot found*/

			sprintf(ret_msg, "Observing <%d>\n", game_id);
			write_return(reg_users[uid].sockfd);
			game_list[game_id].observers[i] = uid;
			print_game_table(game_id, false, uid);
			return;
		}
	}

	reg_users[uid].observeids[ob_loc] = -1;
	sprintf(ret_msg, "The Observer list is full\n");
	write_return(reg_users[uid].sockfd);
	return;
}

void unobserve_game(int uid) {
	char *gid_str, *rest;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	gid_str = __strtok_r(temp_cmd, " ", &rest);
	gid_str = __strtok_r(NULL, " ", &rest);

	if (gid_str == NULL) {
		sprintf(ret_msg, "Please enter a game id\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	int game_id = atoi(gid_str);
	if (game_list[game_id].in_use == false) {
		sprintf(ret_msg, "Please check the game id\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	for (int i = 0; i < MAXCONN; i++) {
		if (game_list[game_id].observers[i] == uid) {
			/*Empty spot found*/
			game_list[game_id].observers[i] = -1;

			sprintf(ret_msg, "Unobserving <%d>\n", game_id);
			write_return(reg_users[uid].sockfd);
			for (int j = 0; j < MAXCONN; j++) {
				if (reg_users[uid].observeids[j] == game_id) {
					/*Resetting players observing list*/
					reg_users[uid].observeids[j] = -1;
				}
			}
			return;
		}
	}
	sprintf(ret_msg, "You are not observing <%d>\n", game_id);
	write_return(reg_users[uid].sockfd);
	return;
}

void refresh_game(int uid) {
	bool loc_obs = false;
	for (int i = 0; i < MAXCONN; i++) {
		if (reg_users[uid].observeids[i] != -1) {
			loc_obs = true;
			print_game_table(reg_users[uid].observeids[i], false, uid);
		}
	}
	if (reg_users[uid].playing == true) {
		loc_obs = true;
		print_game_table(reg_users[uid].gameid, false, uid);
	}

	if (loc_obs == false) {
		sprintf(ret_msg, "You are not observing or playing any games\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
}

void reset_game(int gid) {
	/*Reset the game if someone was playing*/
	game_list[gid].in_use = false;
	game_list[gid].num_moves = 0;
	game_list[gid].player1 = -1;
	game_list[gid].player2 = -1;
	game_list[gid].p1_move = false;
	game_list[gid].p2_move = false;
	game_list[gid].p1_color = -1;
	game_list[gid].p2_color = -1;
	game_list[gid].p1_time = 0;
	game_list[gid].p2_time = 0;
	game_list[gid].game_table[0][0] = -1;
	game_list[gid].game_table[0][1] = -1;
	game_list[gid].game_table[0][2] = -1;
	game_list[gid].game_table[1][0] = -1;
	game_list[gid].game_table[1][1] = -1;
	game_list[gid].game_table[1][2] = -1;
	game_list[gid].game_table[2][0] = -1;
	game_list[gid].game_table[2][1] = -1;
	game_list[gid].game_table[2][2] = -1;
	for (int j = 0; j < MAXCONN; j++) {
		if (game_list[gid].observers[j] != -1) {
			/*Remove gid from persons list*/
			int pid = game_list[gid].observers[j];
			for (int k = 0; k < MAXCONN; k++) {
				if (reg_users[k].observeids[k] == gid) {
					reg_users[k].observeids[k] = -1;
				}
			}
		}
		game_list[gid].observers[j] = -1;
	}
}

int test_game_condition(int gid) {
	/*Determines if someone has won/lost or its a draw*/
	/*0 if 0 has won , 1 if 1 has won, 2 for draw and -1 for none*/
	if (((game_list[gid].game_table[0][0] == 0) && (game_list[gid].game_table[0][1] == 0) && (game_list[gid].game_table[0][2] == 0))
			|| ((game_list[gid].game_table[1][0] == 0) && (game_list[gid].game_table[1][1] == 0) && (game_list[gid].game_table[1][2] == 0))
			|| ((game_list[gid].game_table[2][0] == 0) && (game_list[gid].game_table[2][1] == 0) && (game_list[gid].game_table[2][2] == 0))
			|| ((game_list[gid].game_table[0][0] == 0) && (game_list[gid].game_table[1][0] == 0) && (game_list[gid].game_table[2][0] == 0))
			|| ((game_list[gid].game_table[0][1] == 0) && (game_list[gid].game_table[1][1] == 0) && (game_list[gid].game_table[2][1] == 0))
			|| ((game_list[gid].game_table[0][2] == 0) && (game_list[gid].game_table[1][2] == 0) && (game_list[gid].game_table[2][2] == 0))
			|| ((game_list[gid].game_table[0][0] == 0) && (game_list[gid].game_table[1][1] == 0) && (game_list[gid].game_table[2][2] == 0))
			|| ((game_list[gid].game_table[0][2] == 0) && (game_list[gid].game_table[1][1] == 0) && (game_list[gid].game_table[2][0] == 0))) {
		/*The player with 0 has won*/
		return 0;
	}

	if (((game_list[gid].game_table[0][0] == 1) && (game_list[gid].game_table[0][1] == 1) && (game_list[gid].game_table[0][2] == 1))
			|| ((game_list[gid].game_table[1][0] == 1) && (game_list[gid].game_table[1][1] == 1) && (game_list[gid].game_table[1][2] == 1))
			|| ((game_list[gid].game_table[2][0] == 1) && (game_list[gid].game_table[2][1] == 1) && (game_list[gid].game_table[2][2] == 1))
			|| ((game_list[gid].game_table[0][0] == 1) && (game_list[gid].game_table[1][0] == 1) && (game_list[gid].game_table[2][0] == 1))
			|| ((game_list[gid].game_table[0][1] == 1) && (game_list[gid].game_table[1][1] == 1) && (game_list[gid].game_table[2][1] == 1))
			|| ((game_list[gid].game_table[0][2] == 1) && (game_list[gid].game_table[1][2] == 1) && (game_list[gid].game_table[2][2] == 1))
			|| ((game_list[gid].game_table[0][0] == 1) && (game_list[gid].game_table[1][1] == 1) && (game_list[gid].game_table[2][2] == 1))
			|| ((game_list[gid].game_table[0][2] == 1) && (game_list[gid].game_table[1][1] == 1) && (game_list[gid].game_table[2][0] == 1))) {
		/*The player with 1 has won*/
		return 1;
	}

	if (game_list[gid].num_moves == 9) {
		/*The game has drawn*/
		return 2;
	}

	/*Nothing special has happened*/
	return -1;
}

void set_rating(int uid) {
	reg_users[uid].rating = (reg_users[uid].wins * 1.0f) / (reg_users[uid].wins + reg_users[uid].losses + reg_users[uid].draws);
}

void make_move(int uid) {
	char *cmd, *rest;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	cmd = __strtok_r(temp_cmd, " ", &rest);

	if (reg_users[uid].playing == false) {
		sprintf(ret_msg, "You are not playing a game\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	int gid = reg_users[uid].gameid;

	if (game_list[gid].p1_move == true) {
		game_list[gid].p1_time -= difftime(time(0), game_list[gid].p1_start);
		game_list[gid].p1_start = time(0);
	} else {
		game_list[gid].p2_time -= difftime(time(0), game_list[gid].p2_start);
		game_list[gid].p2_start = time(0);
	}

	if (game_list[gid].p1_time < 0) {
		/*Player 2 has won*/
		print_game_table(gid, true, -1);
		sprintf(ret_msg, "\n%s has won game %d due to timeout\n", reg_users[game_list[gid].player2].username, gid);
		reg_users[game_list[gid].player1].losses++;
		reg_users[game_list[gid].player2].wins++;

		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[i].observers[i] != -1) {
				write_return(reg_users[game_list[i].observers[i]].sockfd);
				write_client_id(reg_users[game_list[i].observers[i]].sockfd, reg_users[game_list[i].observers[i]].username, reg_users[game_list[i].observers[i]].cmd_counter);
			}
		}

		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
		write_client_id(reg_users[game_list[gid].player1].sockfd, reg_users[game_list[gid].player1].username, reg_users[game_list[gid].player1].cmd_counter);
		write_client_id(reg_users[game_list[gid].player2].sockfd, reg_users[game_list[gid].player2].username, reg_users[game_list[gid].player2].cmd_counter);
		reg_users[game_list[gid].player1].playing = false;
		reg_users[game_list[gid].player2].playing = false;
		set_rating(game_list[gid].player1);
		set_rating(game_list[gid].player2);
		reset_game(gid);
		return;
	} else if (game_list[gid].p2_time < 0) {
		/*Player 1 has won*/
		print_game_table(gid, true, -1);
		sprintf(ret_msg, "\n%s has won game %d due to timeout\n", reg_users[game_list[gid].player1].username, gid);
		reg_users[game_list[gid].player1].wins++;
		reg_users[game_list[gid].player2].losses++;

		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[i].observers[i] != -1) {
				write_return(reg_users[game_list[i].observers[i]].sockfd);
				write_client_id(reg_users[game_list[i].observers[i]].sockfd, reg_users[game_list[i].observers[i]].username, reg_users[game_list[i].observers[i]].cmd_counter);

			}
		}

		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
		reg_users[game_list[gid].player1].playing = false;
		reg_users[game_list[gid].player2].playing = false;
		set_rating(game_list[gid].player1);
		set_rating(game_list[gid].player2);
		reset_game(gid);
		return;
	}

	if ((game_list[gid].player1 == uid && game_list[gid].p1_move == false) || (game_list[gid].player2 == uid && game_list[gid].p2_move == false)) {
		/*Not the players turn*/
		sprintf(ret_msg, "Its not your turn.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	int pos1 = -1, pos2 = -1;
	switch (cmd[0]) {
	case 'a':
	case 'A':
		switch (cmd[1]) {
		case '1':
			pos1 = 0;
			pos2 = 0;
			break;
		case '2':
			pos1 = 0;
			pos2 = 1;
			break;
		case '3':
			pos1 = 0;
			pos2 = 2;
			break;
		}
		break;
	case 'b':
	case 'B':
		switch (cmd[1]) {
		case '1':
			pos1 = 1;
			pos2 = 0;
			break;
		case '2':
			pos1 = 1;
			pos2 = 1;
			break;
		case '3':
			pos1 = 1;
			pos2 = 2;
			break;
		}
		break;
	case 'c':
	case 'C':
		switch (cmd[1]) {
		case '1':
			pos1 = 2;
			pos2 = 0;
			break;
		case '2':
			pos1 = 2;
			pos2 = 1;
			break;
		case '3':
			pos1 = 2;
			pos2 = 2;
			break;
		}
		break;
	}

	if (game_list[gid].game_table[pos1][pos2] != -1) {
		/*Position already full*/
		sprintf(ret_msg, "<Position is occupied>.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	/*Position available, so place the stuff*/
	int p_color = ((game_list[gid].player1 == uid) ? game_list[gid].p1_color : game_list[gid].p2_color);
	game_list[gid].game_table[pos1][pos2] = p_color;

	printf("Player %s played pos1 = %d pos2 = %d", reg_users[uid].username, pos1, pos2);
	if ((game_list[gid].player1 == uid)) {
		game_list[gid].p1_move = false;
		game_list[gid].p2_move = true;
		game_list[gid].p2_start = time(0);
	} else {
		game_list[gid].p1_move = true;
		game_list[gid].p2_move = false;
		game_list[gid].p1_start = time(0);
	}

	printf("\n%s moving %d to (%d,%d)\n", reg_users[uid].username, p_color, pos1, pos2);
	int stat = test_game_condition(gid);
	print_game_table(gid, true, -1);
	if (stat == 0) {
		/*Player 0 has won*/
		sprintf(ret_msg, "\n%s has won game %d\n", ((game_list[gid].p1_color == 0) ? (reg_users[game_list[gid].player1].username) : (reg_users[game_list[gid].player2].username)), gid);
		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[gid].observers[i] != -1) {
				write_return(reg_users[game_list[gid].observers[i]].sockfd);
				write_client_id(reg_users[game_list[gid].observers[i]].sockfd, reg_users[game_list[gid].observers[i]].username, reg_users[game_list[gid].observers[i]].cmd_counter);
			}
		}

		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
		/*Update stats*/
		if (game_list[gid].p1_color == 0) {
			reg_users[game_list[gid].player1].wins++;
			reg_users[game_list[gid].player2].losses++;
		} else {
			reg_users[game_list[gid].player2].wins++;
			reg_users[game_list[gid].player1].losses++;
		}
		reg_users[game_list[gid].player1].playing = false;
		reg_users[game_list[gid].player2].playing = false;
		set_rating(game_list[gid].player1);
		set_rating(game_list[gid].player2);
		reset_game(gid);
		return;
	} else if (stat == 1) {
		/*Player 1 has won*/
		sprintf(ret_msg, "\n%s has won game %d\n", ((game_list[gid].p1_color == 1) ? (reg_users[game_list[gid].player1].username) : (reg_users[game_list[gid].player2].username)), gid);
		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[gid].observers[i] != -1) {
				write_return(reg_users[game_list[gid].observers[i]].sockfd);
				write_client_id(reg_users[game_list[gid].observers[i]].sockfd, reg_users[game_list[gid].observers[i]].username, reg_users[game_list[gid].observers[i]].cmd_counter);
			}
		}
		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
		/*Update stats*/
		if (game_list[gid].p1_color == 1) {
			reg_users[game_list[gid].player1].wins++;
			reg_users[game_list[gid].player2].losses++;
		} else {
			reg_users[game_list[gid].player2].wins++;
			reg_users[game_list[gid].player1].losses++;
		}
		reg_users[game_list[gid].player1].playing = false;
		reg_users[game_list[gid].player2].playing = false;
		set_rating(game_list[gid].player1);
		set_rating(game_list[gid].player2);
		reset_game(gid);
		return;
	} else if (stat == 2) {
		/*The game has drawn*/
		sprintf(ret_msg, "\nThe game %d has drawn\n", gid);
		for (int i = 0; i < MAXCONN; i++) {
			if (game_list[gid].observers[i] != -1) {
				write_return(reg_users[game_list[gid].observers[i]].sockfd);
				write_client_id(reg_users[game_list[gid].observers[i]].sockfd, reg_users[game_list[gid].observers[i]].username, reg_users[game_list[gid].observers[i]].cmd_counter);
			}
		}
		write_return(reg_users[game_list[gid].player1].sockfd);
		write_return(reg_users[game_list[gid].player2].sockfd);
		/*Update stats*/
		reg_users[game_list[gid].player1].draws++;
		reg_users[game_list[gid].player2].draws++;
		reg_users[game_list[gid].player1].playing = false;
		reg_users[game_list[gid].player2].playing = false;
		set_rating(game_list[gid].player1);
		set_rating(game_list[gid].player2);
		reset_game(gid);
		return;
	} else {
		/*Nothing fancy happened*/
		game_list[gid].num_moves++;
		if (game_list[gid].player1 != uid) {
			write_client_id(reg_users[game_list[gid].player1].sockfd, reg_users[game_list[gid].player1].username, reg_users[game_list[gid].player1].cmd_counter);
		} else {
			write_client_id(reg_users[game_list[gid].player2].sockfd, reg_users[game_list[gid].player2].username, reg_users[game_list[gid].player2].cmd_counter);
		}
	}

}

void resign_game(int uid) {
	if (reg_users[uid].playing == false) {
		sprintf(ret_msg, "You are not playing a game\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	int gid = reg_users[uid].gameid;
	if (game_list[gid].player1 == uid) {
		/*Player 1 has quit*/
		reg_users[game_list[gid].player1].losses++;
		reg_users[game_list[gid].player2].wins++;
	} else {
		/*Player 2 has quit*/
		reg_users[game_list[gid].player1].wins++;
		reg_users[game_list[gid].player2].losses++;
		return;
	}
	sprintf(ret_msg, "\n<%s resigned>\n", reg_users[uid].username);
	for (int i = 0; i < MAXCONN; i++) {
		if (game_list[gid].observers[i] != -1) {
			write_return(reg_users[game_list[gid].observers[i]].sockfd);
			write_client_id(reg_users[game_list[gid].observers[i]].sockfd, reg_users[game_list[gid].observers[i]].username, reg_users[game_list[gid].observers[i]].cmd_counter);
		}
	}
	write_return(reg_users[game_list[gid].player1].sockfd);
	write_return(reg_users[game_list[gid].player2].sockfd);
	reg_users[game_list[gid].player1].playing = false;
	reg_users[game_list[gid].player2].playing = false;
	reg_users[game_list[gid].player1].gameid = -1;
	reg_users[game_list[gid].player2].gameid = -1;
	set_rating(game_list[gid].player1);
	set_rating(game_list[gid].player2);
	reset_game(gid);
}

void kibitz_message(int uid) {
	bool observing_flag = false;
	char *cmd, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	cmd = __strtok_r(temp_cmd, " ", &msg);

	if (msg == NULL) {
		sprintf(ret_msg, "Please enter a message\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	for (int i = 0; i < MAXCONN; i++) {
		if (reg_users[uid].observeids[i] != -1) {
			/*Found a game the player is observing*/
			observing_flag = true;
			int gid = reg_users[uid].observeids[i];
			printf("Found game = %d\n", gid);
			for (int j = 0; j < MAXCONN; j++) {
				/*Send message to all observes if not blocked or quiet*/
				if (game_list[gid].observers[j] != -1) {
					/*Game observe found*/
					int pid = game_list[gid].observers[j];
					if (check_blocked(pid, uid) == false) {
						printf("Found player = %d\n", pid);
						sprintf(ret_msg, "Kibitz* %s: %s\n", reg_users[uid].username, msg);
						write_return(reg_users[pid].sockfd);
						write_client_id(reg_users[pid].sockfd, reg_users[pid].username, reg_users[pid].cmd_counter);
					}
				}
			}
		}
	}

	if (observing_flag == false) {
		sprintf(ret_msg, "You are not observing any games\n");
		write_return(reg_users[uid].sockfd);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void time_to_string(char *str_time) {
	time_t t = time(0);
	strftime(str_time, 100, "%a %b %d %H:%M:%S %Y ", localtime(&t));
}

void print_num_unread_msg(int uid) {
	int total = 0;
	for (int i = 0; i < NUMMSG; i++) {
		if (reg_users[uid].mail_list[i].read_status == false && reg_users[uid].mail_list[i].isfilled == true) {
			total++;
		}
	}
	if (total > 0) {
		sprintf(ret_msg, "You have %d unread messages.\n", total);
		write_return(reg_users[uid].sockfd);
		return;
	}
}

void list_mail(int uid) {
	sprintf(ret_msg, "!MAILBOX!\n");
	for (int i = 0; i < NUMMSG; i++) {
		if (i == 0 && reg_users[uid].mail_list[i].isfilled == false) {
			sprintf(ret_msg + strlen(ret_msg), "You have no messages.\n");
			write_return(reg_users[uid].sockfd);
			return;
		}
		if (reg_users[uid].mail_list[i].isfilled == true) {
			sprintf(ret_msg + strlen(ret_msg), "%2d  %6s  %10s  \" %s \" %s\n", i, (reg_users[uid].mail_list[i].read_status == true) ? ("Read") : ("Unread"), reg_users[uid].mail_list[i].from_username,
					reg_users[uid].mail_list[i].title, reg_users[uid].mail_list[i].timestamp);
		}
	}
	write_return(reg_users[uid].sockfd);
}

void read_mail_msg(int uid) {
	printf("Reading msg\n");
	fflush(0);
	if (strcmp(".", usr_msg) == 0) {
		int ret = reg_users[uid].temp_sending_to;
		printf("Message ended\n");
		fflush(0);
		reg_users[uid].sending_mail = false;
		/*Send the mail here*/
		/*Add the mail to the mailbox*/
		for (int i = 0; i < NUMMSG; i++) {
			if (reg_users[ret].mail_list[i].isfilled == false) {
				strcpy(reg_users[ret].mail_list[i].to_username, reg_users[ret].username);
				strcpy(reg_users[ret].mail_list[i].from_username, reg_users[uid].username);
				strcpy(reg_users[ret].mail_list[i].title, reg_users[uid].temp_title);
				strcpy(reg_users[ret].mail_list[i].timestamp, reg_users[uid].temp_timestamp);
				strcpy(reg_users[ret].mail_list[i].text, reg_users[uid].message_body);
				reg_users[ret].mail_list[i].read_status = false;
				reg_users[ret].mail_list[i].isfilled = true;

				if (reg_users[ret].isloggedin == true) {
					sprintf(ret_msg, "\nYou have received a  new message \n");
					write_return(reg_users[ret].sockfd);
					write_client_id(reg_users[ret].sockfd, reg_users[ret].username, reg_users[ret].cmd_counter);
				}
				sprintf(ret_msg,"Your message has been sent\n");
				write_return(reg_users[uid].sockfd);
				write_client_id(reg_users[uid].sockfd, reg_users[uid].username, reg_users[uid].cmd_counter);
				strcpy(usr_msg, "");
				strcpy(reg_users[uid].message_body, "");
				return;
			}
		}

		strcpy(reg_users[uid].message_body, "");
		sprintf(ret_msg, "%s Mailbox is full.", reg_users[uid].username);
		write_return(reg_users[uid].sockfd);
		strcpy(usr_msg, "");
		return;
	}
	if (strcmp(reg_users[uid].message_body, "NULL") == 0) {
		/*First word so remove that null*/
		strcpy(reg_users[uid].message_body, usr_msg);
		strcat(reg_users[uid].message_body, "\n");
	} else {
		strcat(reg_users[uid].message_body, usr_msg);
		strcat(reg_users[uid].message_body, "\n");
	}
}

void send_mail(int uid) {
	char *username, *cmd, *msg, *rest;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	username = __strtok_r(temp_cmd, " ", &rest);
	username = __strtok_r(NULL, " ", &rest);

	printf("Telling username = %s\t msg = %s\n", username, msg);
	fflush(0);
	fflush(stdout);
	int ret = find_user(username);
	if (ret == -1) {
		/*User not found*/
		sprintf(ret_msg, "User not found, please check the username\n");
		write_return(reg_users[uid].sockfd);
		write_client_id(reg_users[uid].sockfd, reg_users[uid].username, reg_users[uid].cmd_counter);
		return;
	}

	if (check_blocked(ret, uid) == true) {
		sprintf(ret_msg, "You cannot mail to %s. You are blocked\n", username);
		write_return(reg_users[uid].sockfd);
		write_client_id(reg_users[uid].sockfd, reg_users[uid].username, reg_users[uid].cmd_counter);
		return;
	}

	msg = __strtok_r(NULL, "\n", &rest);
	if (msg == NULL) {
		sprintf(ret_msg, "Your mail does not have a title\n");
		write_return(reg_users[uid].sockfd);
		write_client_id(reg_users[uid].sockfd, reg_users[uid].username, reg_users[uid].cmd_counter);
		return;
	}

	bool sending_mail;
	int temp_sending_to;
	char temp_title;
	char temp_timestamp;

	/*Send the message*/
	reg_users[uid].sending_mail = true;
	reg_users[uid].temp_sending_to = ret;
	strcpy(reg_users[uid].temp_title, msg);
	time_to_string(reg_users[uid].temp_timestamp);

	sprintf(ret_msg, "Please input mail body, using only '.' for the final line\n");
	write_return(reg_users[uid].sockfd);
	return;
}

void read_mail(int uid) {
	char *msg_num, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	msg_num = __strtok_r(temp_cmd, " ", &msg);
	msg_num = __strtok_r(NULL, " ", &msg);

	if (msg_num == NULL) {
		sprintf(ret_msg, "Message number invalid.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	int mnum = atoi(msg_num);
	if (reg_users[uid].mail_list[mnum].isfilled == false) {
		sprintf(ret_msg, "Message number invalid.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	/*Writing the mail to the user*/
	reg_users[uid].mail_list[mnum].read_status = true;
	sprintf(ret_msg, "From: %s\nTitle: %s\nTime: %s\n%s\n", reg_users[uid].mail_list[mnum].from_username, reg_users[uid].mail_list[mnum].title, reg_users[uid].mail_list[mnum].timestamp,
			reg_users[uid].mail_list[mnum].text);
	write_return(reg_users[uid].sockfd);
	return;

}

void delete_mail(int uid) {
	char *msg_num, *msg;
	char temp_cmd[MSGSIZE];
	strcpy(temp_cmd, usr_msg);
	msg_num = __strtok_r(temp_cmd, " ", &msg);
	msg_num = __strtok_r(NULL, " ", &msg);
	if (msg_num == NULL) {
		sprintf(ret_msg, "Message number invalid.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}
	int mnum = atoi(msg_num);
	if (reg_users[uid].mail_list[mnum].isfilled == false) {
		sprintf(ret_msg, "Message number invalid.\n");
		write_return(reg_users[uid].sockfd);
		return;
	}

	/*Writing the mail to the user*/
	reg_users[uid].mail_list[mnum].isfilled = false;
	sprintf(ret_msg, "Message deleted.\n");
	write_return(reg_users[uid].sockfd);
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool test_length(int num, bool gt) {
#if brutal==1
	char temp[MSGSIZE];
	strcpy(temp, usr_msg);
	int counter = 0;
	char * newString;
	newString = strtok(usr_msg, " ");
	while (newString != NULL) {
		counter++;
		newString = strtok(NULL, " ");
	}

	if (gt == true) {
		/*Must be equal or greater than num*/
		if (counter >= num)
		return true;
		return false;
	} else {
		/*Must exactly be equal to num*/
		if (counter == num)
		return true;
		return false;
	}
#else
	return true;
#endif
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
		if (test_length(1, false))
			return 0;
		return 28;
	} else if (strcmp(cmd, "stats") == 0) {
		printf("Calling stats\n");
		if (test_length(2, false))
			return 1;
		return 28;
	} else if (strcmp(cmd, "game") == 0) {
		printf("Calling game\n");
		if (test_length(1, false))
			return 2;
		return 28;
	} else if (strcmp(cmd, "observe") == 0) {
		printf("Calling observe\n");
		if (test_length(2, false))
			return 3;
		return 28;
	} else if (strcmp(cmd, "unobserve") == 0) {
		printf("Calling unobserve\n");
		if (test_length(2, false))
			return 4;
		return 28;
	} else if (strcmp(cmd, "match") == 0) {
		printf("Calling match\n");
		if (test_length(2, true))
			return 5;
		return 28;
	} else if (strcmp(cmd, "resign") == 0) {
		printf("Calling resign\n");
		if (test_length(1, false))
			return 6;
		return 28;
	} else if (strcmp(cmd, "refresh") == 0) {
		printf("Calling refresh\n");
		if (test_length(1, false))
			return 7;
		return 28;
	} else if (strcmp(cmd, "shout") == 0) {
		printf("Calling shout\n");
		if (test_length(2, true))
			return 8;
		return 28;
	} else if (strcmp(cmd, "tell") == 0) {
		printf("Calling tell\n");
		if (test_length(3, true))
			return 9;
		return 28;
	} else if (strcmp(cmd, "kibitz") == 0) {
		printf("Calling kibitz\n");
		if (test_length(2, true))
			return 10;
		return 28;
	} else if (strcmp(cmd, "'") == 0) {
		printf("Calling quot\n");
		if (test_length(2, true))
			return 11;
		return 28;
	} else if (strcmp(cmd, "quiet") == 0) {
		printf("Calling quiet\n");
		if (test_length(1, false))
			return 12;
		return 28;
	} else if (strcmp(cmd, "nonquiet") == 0) {
		printf("Calling nonquiet\n");
		if (test_length(1, false))
			return 13;
		return 28;
	} else if (strcmp(cmd, "block") == 0) {
		printf("Calling block\n");
		if (test_length(2, false))
			return 14;
		return 28;
	} else if (strcmp(cmd, "unblock") == 0) {
		printf("Calling unblock\n");
		if (test_length(2, false))
			return 15;
		return 28;
	} else if (strcmp(cmd, "listmail") == 0) {
		printf("Calling listmail\n");
		if (test_length(1, false))
			return 16;
		return 28;
	} else if (strcmp(cmd, "readmail") == 0) {
		printf("Calling readmail\n");
		if (test_length(2, false))
			return 17;
		return 28;
	} else if (strcmp(cmd, "deletemail") == 0) {
		printf("Calling deletemail\n");
		if (test_length(2, false))
			return 18;
		return 28;
	} else if (strcmp(cmd, "mail") == 0) {
		printf("Calling mail\n");
		if (test_length(3, true))
			return 19;
		return 28;
	} else if (strcmp(cmd, "info") == 0) {
		printf("Calling info\n");
		if (test_length(2, true))
			return 20;
		return 28;
	} else if (strcmp(cmd, "passwd") == 0) {
		printf("Calling passwd\n");
		if (test_length(2, false))
			return 21;
		return 28;
	} else if (strcmp(cmd, "exit") == 0) {
		printf("Calling exit\n");
		if (test_length(1, false))
			return 22;
		return 28;
	} else if (strcmp(cmd, "quit") == 0) {
		printf("Calling quit\n");
		if (test_length(1, false))
			return 23;
		return 28;
	} else if (strcmp(cmd, "help") == 0) {
		printf("Calling help\n");
		if (test_length(1, false))
			return 24;
		return 28;
	} else if (strcmp(cmd, "?") == 0) {
		printf("Calling ?\n");
		if (test_length(1, false))
			return 25;
		return 28;
	} else if (strcmp(cmd, "register") == 0) {
		printf("Calling register\n");
		if (test_length(3, false))
			return 26;
		return 28;
	} else if (strcmp(cmd, "\n") == 0) {
		/*Empty line*/
		printf("Empty Line\n");
		return 27;
	} else if ((strcmp(cmd, "a1") == 0) || (strcmp(cmd, "b1") == 0) || (strcmp(cmd, "c1") == 0) || (strcmp(cmd, "a2") == 0) || (strcmp(cmd, "b2") == 0) || (strcmp(cmd, "c2") == 0)
			|| (strcmp(cmd, "a3") == 0) || (strcmp(cmd, "b3") == 0) || (strcmp(cmd, "c3") == 0) || (strcmp(cmd, "A1") == 0) || (strcmp(cmd, "B1") == 0) || (strcmp(cmd, "C1") == 0)
			|| (strcmp(cmd, "A2") == 0) || (strcmp(cmd, "B2") == 0) || (strcmp(cmd, "C2") == 0) || (strcmp(cmd, "A3") == 0) || (strcmp(cmd, "B3") == 0) || (strcmp(cmd, "C3") == 0)) {
		/*A game move*/
		printf("Game move detected\n");
		return 29;
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

	load_db();

	if (reg_table[0].loc == -1) {
		/*Empty list , so load fake stuff*/
		load_files();
	}

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
							if (guest_users[i].entered_username == true) {
								/*Password is a empty line so close connection*/
								printf("Guest %d has left\n", i);
								sprintf(ret_msg, "Login failed!!\nThank you for using Online Tic-tac-toe Server.\nSee you next time.\n");
								write_return(guest_users[i].sockfd);
								FD_CLR(guest_users[i].sockfd, &allset);
								reset_guest(i, true);
							}
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
										if (reg_table[userid].logged_in == true) {
											/*Already logged in, so kick him*/
											reset_game(userid);
											sprintf(ret_msg, "\nYou have been kicked out due to multiple login\n");
											write_return(reg_users[userid].sockfd);
											FD_CLR(reg_users[userid].sockfd, &allset);
											close(reg_users[userid].sockfd);
											reset_client(userid);
										}
										reg_table[userid].logged_in = true;
										reg_users[userid].sockfd = guest_users[i].sockfd;
										reg_users[userid].isloggedin = true;
										reset_guest(i, false);
										reg_users[userid].cmd_counter++;
										printMenu(reg_users[userid].sockfd);
										print_num_unread_msg(userid);
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
							reset_game(i);
							printf("Client %d has left", i);
							reg_table[i].logged_in = false;
							reg_users[i].isloggedin = false;
							reg_users[i].isloggedin = false;
							FD_CLR(reg_users[i].sockfd, &allset);
							reg_users[i].sockfd = -1;
						} else {
							cleanup_usr_msg();
							int ret = match_command();

							if (reg_users[i].sending_mail == true) {
								/*In sendmail mode, so ignore everything else */
								/*and use the special case path*/
								ret = -1;
							}
							switch (ret) {
							case -1:
								/*Writing body now so ignore everything else*/
								read_mail_msg(i);
								/*To counter the addition at the end*/
								reg_users[i].cmd_counter--;
								break;
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
							case 2:
								/*list games*/
								list_games(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);

								break;
							case 3:
								observe_game(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 4:
								unobserve_game(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 5:
								/*match call*/
								setup_match(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 6:
								resign_game(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 7:
								refresh_game(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 8:
								/*Shouting*/
								shout_msg(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 9:
								/*Tell*/
								tell_msg(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 10:
							case 11:
								kibitz_message(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 12:
								/*quiet call*/
								if (reg_users[i].quiet == false) {
									reg_users[i].quiet = true;
									sprintf(ret_msg, "Quiet mode activated\n");
								} else {
									sprintf(ret_msg, "Quiet mode already active\n");
								}
								write_return(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 13:
								/*unquiet call*/
								if (reg_users[i].quiet == true) {
									reg_users[i].quiet = false;
									sprintf(ret_msg, "Quiet mode deactivated\n");
								} else {
									sprintf(ret_msg, "Quiet mode was never active\n");
								}
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
							case 16:
								list_mail(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 17:
								read_mail(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 18:
								delete_mail(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 19:
								/*Mail message*/
								send_mail(i);
								break;
							case 20:
								/*Info*/
								set_info(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 21:
								/*password*/
								set_passwd(i);
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
								reset_client(i);
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
								write_return(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 27:
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 28:
								write_wrong_syntax(reg_users[i].sockfd);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							case 29:
								make_move(i);
								write_client_id(reg_users[i].sockfd, reg_users[i].username, reg_users[i].cmd_counter);
								break;
							}
							store_db();
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
