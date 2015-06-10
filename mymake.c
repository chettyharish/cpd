#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define search_path_env "PATH"
#define STLEN 200
#define NUMELE 50
#define empty_cmd -1
#define norm_cmd 0
#define pipe_cmd 1
#define pipe_cmd_last 2
#define redr_cmd 3
#define redr_both_cmd 4
#define cdir_cmd 5
#define back_cmd 6
#define mult_cmd 7
#define mult_cmd_last 8
#define echo_cmd 9
#define empty_target 20
#define norm_target 21
#define infr_target 22

struct userinput {
	char make_file_name[STLEN];
	char target[STLEN];
	char cdir[STLEN];
	bool print;
	bool force;
	bool debug;
	bool interrupt;
	int time;
} ui;

/*Internal counter for counting different elements
 * Can cont multiple times in case of multiline commands*/
struct counters {
	int macros;
	int targets;
	int inferences;
	int names;
	int commands;
} counters;

/*Real count*/
struct print_counter {
	int macros;
	int targets;
	int inferences;
	int names;
	int commands;
} print_counter;

struct command_line {
	char com[STLEN];
	int command_type;
	int sp_command_type; // Backup for multiline OR piped commands
};

struct command_list {
	struct command_line list[NUMELE];
	int command_count;
};

struct targets {
	char target_name[STLEN];
	int dependency_count;
	char dependecies[NUMELE][STLEN];
	int target_type;
	char inference_from[STLEN];
	char inference_to[STLEN];
	struct command_list commands;
};

struct macros {
	char macro[STLEN];
	char macro_replace[STLEN];
};

struct queue {
	char targ_queue[NUMELE][STLEN];
	bool visited[NUMELE];
	int queue_end;
} queue;

struct targets *target_arr;
struct macros *macro_arr;
struct command_line *cmd_list;
char *s = "    ";
char type_buf[STLEN];

char* print_type(int type) {

	switch (type) {
	case empty_target:
		strcpy(type_buf, "empty_target");
		return type_buf;
	case norm_target:
		strcpy(type_buf, "empty_target");
		return type_buf;
	case infr_target:
		strcpy(type_buf, "empty_target");
		return type_buf;
	case empty_cmd:
		strcpy(type_buf, "empty_cmd");
		return type_buf;
	case norm_cmd:
		strcpy(type_buf, "norm_cmd");
		return type_buf;
	case pipe_cmd_last:
	case pipe_cmd:
		strcpy(type_buf, "pipe_cmd");
		return type_buf;
	case redr_both_cmd:
		strcpy(type_buf, "redr_both_cmd");
		return type_buf;
	case redr_cmd:
		strcpy(type_buf, "redr_cmd");
		return type_buf;
	case cdir_cmd:
		strcpy(type_buf, "cdir_cmd");
		return type_buf;
	case back_cmd:
		strcpy(type_buf, "back_cmd");
		return type_buf;
	case mult_cmd_last:
	case mult_cmd:
		strcpy(type_buf, "mult_cmd");
		return type_buf;
	case echo_cmd:
		strcpy(type_buf, "echo_cmd");
		return type_buf;
	}

	strcpy(type_buf, "NEVER HAPPENS");
	return type_buf;
}
void trim_string(char *temp) {
	int front_spaces = 0;
	int back_spaces = 0;
	char new_str[STLEN];

	for (int i = 0; i < strlen(temp); i++) {
		if (temp[i] == ' ') {
			front_spaces++;
		} else {
			break;
		}
	}

	for (int i = strlen(temp); i > 0; i--) {
		if (temp[i] == '\0' || temp[i] == ' ' || temp[i] == '\n') {
			back_spaces++;
		} else {
			break;
		}
	}
	strncpy(new_str, temp + front_spaces, strlen(temp) - front_spaces - back_spaces + 1);
	new_str[strlen(temp) - front_spaces - back_spaces + 1] = '\0';
	strcpy(temp, new_str);
}

bool test_last_char(char *buffer_temp, char val) {
	/*Trying to test if last char is required char*/
	char temp[STLEN];
	strcpy(temp, buffer_temp);
	trim_string(temp);
	if (temp[strlen(temp) - 1] == val)
		return true;
	return false;
}

void remove_newline(char *temp) {
	char *pos;
	if ((pos = strchr(temp, '\n'))) {
		/*To remove \n !*/
		*pos = '\0';
	}

}

void print_dir() {
	char temp[STLEN];
	getcwd(temp, sizeof(temp));
	printf("Directory : %s\n", temp);
}

int find_file(char *env_path, char* cmd) {
	/*Returns 0 if succesful else 1*/
	struct stat buf;
	char file_path[STLEN];
	char temp_env[STLEN];
	char temp_cmd[STLEN];
	char buffer[STLEN];
	char *pos;
	strcpy(temp_env, env_path);
	strcpy(temp_cmd, cmd);
	char *file_name = strtok(temp_cmd, " ");

	if (strlen(cmd) == 0) {
		/*Empty line so don't do anything*/
		return 0;
	} else if (file_name[0] == '/') {
		/*Absolute path found dont change anything*/
		return 0;
	} else if (strcmp(file_name, "cd") == 0 || strcmp(file_name, "echo") == 0) {
		/*CD or ECHO command, will be handled later!*/
		return 0;
	} else {
		char *path = strtok(temp_env, ":");
		while (path != NULL) {
			/*Dynamic path probably, so search MYPATH*/
			sprintf(file_path, "%s/%s", path, file_name);
			if (stat(file_path, &buf) == 0) {
				if ((pos = strstr(cmd, file_name))) {
					strncpy(buffer, cmd, pos - cmd);
					sprintf(buffer + (pos - cmd), "%s%s", file_path, pos + strlen(file_name));
					strcpy(cmd, buffer);
				}
				return 0;
			}
			path = strtok(NULL, ":");
		}
	}

	printf("File %s not found!!!", file_name);

	return 1;

}

int test_targ_type(char *t_name) {
	char temp[STLEN];
	char *pos;
	int dot_count = 0;
	strcpy(temp, t_name);
	trim_string(temp);
	if ((pos = strchr(temp, '.'))) {
		if (pos - temp == 0) {
			/*Inference found as the rule starts with a '.'*/
			for (int i = 0; i < strlen(temp); i++) {
				if (temp[i] == '.') {
					dot_count++;
				}
			}

			if (dot_count == 1) {
				return 1;
			} else if (dot_count == 2) {
				return 2;
			} else {
				printf("Wrong Inference Target\n");
			}
		} else {
			/*Since there are stuff before '.' , it is not inference*/
			return norm_target;
		}
	} else {
		/*Since target doesn't have '.' , it cannot be inference*/
		return norm_target;
	}

	return norm_target;
}

void tokenize(char *buffer, char *exec_args[]) {
	int counter = 0;
	char buffer_temp[STLEN];
	strcpy(buffer_temp, buffer);
	char *token = strtok(buffer_temp, " ");
	while (token != NULL) {
		exec_args[counter] = token;
		counter++;
		token = strtok(NULL, " ");
	}

	if (test_last_char(buffer, '&')) {
		/*This removes the &, so that it is not assumed as an argument*/
		exec_args[counter - 1] = NULL;
	}
	exec_args[counter] = NULL;
}

void remove_tab(char *temp) {
	/*Removes the leading Tab character*/
	if (temp[0] == '\t') {
		temp[0] = ' ';
	}
	trim_string(temp);
}

int find_target_idx(char *target) {
	int matched_target = -1;
	for (int i = 0; i < counters.targets; i++) {
		if (strcmp(target, target_arr[i].target_name) == 0) {
			matched_target = i;
			return matched_target;
		}
	}

	if (matched_target == -1) {
		/*Search for matching inference rules*/
		for (int i = 0; i < counters.targets; i++) {
			if (target_arr[i].target_type == infr_target) {
				char f_name[STLEN];
				struct stat buf;
				sprintf(f_name, "%s%c%s", target, '.', target_arr[i].inference_from);

				if (stat(f_name, &buf) == 0) {
					matched_target = i;
					printf("%s \t\tMATCH FOUND %s \n", target, target_arr[i].target_name);
					return matched_target;
				}
			}
		}
	}

	return -1;
}

void dfs(char *target) {
	int idx = find_target_idx(target);
	if (strlen(target) == 0)
		return;
	if (idx == -1) {
		/*Test if it is a file in the current directory*/
		struct stat buf;
		if (stat(target, &buf) == 0) {
			strcpy(queue.targ_queue[queue.queue_end++], target);
		} else {
			printf("Missing Target : %s\n", target);
		}
		return;
	}

	if (queue.visited[idx] == false) {
		queue.visited[idx] = true;

		for (int i = 0; i < target_arr[idx].dependency_count; i++) {
			dfs(target_arr[idx].dependecies[i]);
		}
		strcpy(queue.targ_queue[queue.queue_end++], target_arr[idx].target_name);
	}
}

void get_macro(char *buffer_temp) {
	char *macro = strtok(buffer_temp, "=");
	char *macro_replace = strtok(NULL, "\n");

	sprintf(macro_arr[counters.macros].macro, "$(%s)", macro);
	sprintf(macro_arr[counters.macros].macro_replace, "%s", macro_replace);
	counters.macros++;
	sprintf(macro_arr[counters.macros].macro, "$%s", macro);
	sprintf(macro_arr[counters.macros].macro_replace, "%s", macro_replace);
	counters.macros++;
}

void wait_all_children() {
	while (wait(NULL) > 0)
		;
}

void get_target(char *buffer_temp, int target_pos) {

	counters.targets++;
	char *target = strtok(buffer_temp, ":");
	trim_string(target);
	strcpy(target_arr[target_pos].target_name, target);
	char *dependencies = strtok(NULL, " ");

	while (dependencies != NULL) {
		remove_newline(dependencies);
		strcpy(target_arr[target_pos].dependecies[target_arr[target_pos].dependency_count++], dependencies);
		dependencies = strtok(NULL, " ");
	}

	/*Have to set type of Target here*/
	if (test_targ_type(target_arr[target_pos].target_name) == norm_target) {
		print_counter.targets++;
		target_arr[target_pos].target_type = norm_target;
		strcpy(target_arr[target_pos].inference_from, "\0");
		strcpy(target_arr[target_pos].inference_to, "\0");
	} else if (test_targ_type(target_arr[target_pos].target_name) == 1) {
		print_counter.inferences++;
		char temp[STLEN];
		strcpy(temp, target_arr[target_pos].target_name);
		char *from = strtok(temp, ".");
		char *to = "\0";
		strcpy(target_arr[target_pos].inference_from, from);
		strcpy(target_arr[target_pos].inference_to, to);
		target_arr[target_pos].target_type = infr_target;
	} else if (test_targ_type(target_arr[target_pos].target_name) == 2) {
		print_counter.inferences++;
		char temp[STLEN];
		strcpy(temp, target_arr[target_pos].target_name);
		char *from = strtok(temp, ".");
		char *to = strtok(NULL, " :");
		strcpy(target_arr[target_pos].inference_from, from);
		strcpy(target_arr[target_pos].inference_to, to);
		target_arr[target_pos].target_type = infr_target;
	}
}

void replace_macros(char *buffer_temp) {
	char buffer[STLEN];
	char *pos;
	for (int i = 0; i < counters.macros; i++) {
		/*Testing each macro whether it exists or not*/
		while ((pos = strstr(buffer_temp, macro_arr[i].macro))) {
			strncpy(buffer, buffer_temp, pos - buffer_temp);
			sprintf(buffer + (pos - buffer_temp), "%s%s", macro_arr[i].macro_replace, pos + strlen(macro_arr[i].macro));
			strcpy(buffer_temp, buffer);
		}
	}

	/*Do the replacements only for non inference targets*/
	/*Need to resolve inference ones at execution*/
	/*Need to resolve macros for $@ and $<*/
	while ((pos = strstr(buffer_temp, "$@"))) {
		strncpy(buffer, buffer_temp, pos - buffer_temp);
		if (target_arr[counters.targets - 1].target_type == norm_target) {
			sprintf(buffer + (pos - buffer_temp), "%s%s", target_arr[counters.targets - 1].target_name, pos + strlen("$@"));
		} else if (target_arr[counters.targets - 1].target_type == infr_target) {
			/*Changing the inference symbols to something else
			 * to ensure that char '<' does not end up matching redr_cmd*/
			sprintf(buffer + (pos - buffer_temp), "%s%s", "$(TARGET)", pos + strlen("$@"));
		}
		strcpy(buffer_temp, buffer);
	}

	while ((pos = strstr(buffer_temp, "$<"))) {
		strncpy(buffer, buffer_temp, pos - buffer_temp);
		if (target_arr[counters.targets - 1].target_type == norm_target) {
			if (target_arr[counters.targets - 1].dependency_count != 0) {
				sprintf(buffer + (pos - buffer_temp), "%s%s", target_arr[counters.targets - 1].dependecies[0], pos + strlen("$<"));
			} else {
				printf("Makefile error $<");
			}
		} else if (target_arr[counters.targets - 1].target_type == infr_target) {
			/*Changing the inference symbols to something else
			 * to ensure that char '<' does not end up matching redr_cmd*/
			sprintf(buffer + (pos - buffer_temp), "%s%s", "$(DEP)", pos + strlen("$<"));
		}
		strcpy(buffer_temp, buffer);
	}

}

bool test_cdir_cmd(char *cmd) {
	if (strlen(cmd) == 0) {
		/*Empty command, so just return!*/
		return false;
	}
	char buffer_temp[STLEN];
	strcpy(buffer_temp, cmd);
	char *pos = strtok(buffer_temp, " ");
	if (strcmp(pos, "cd") == 0) {
		printf("%-80s CD Command\n", cmd);
		return true;
	}
	return false;
}

bool test_echo_cmd(char *cmd) {
	if (strlen(cmd) == 0) {
		/*Empty command, so just return!*/
		return false;
	}
	char buffer_temp[STLEN];
	strcpy(buffer_temp, cmd);
	char *pos = strtok(buffer_temp, " ");
	if (strcmp(pos, "echo") == 0) {
		printf("%-80s ECHO Command\n", cmd);
		return true;
	}
	return false;
}

void get_cmd(char *buffer_temp, int target_pos) {
	/*Have to set type of Target here*/
	replace_macros(buffer_temp);
	remove_newline(buffer_temp);
	remove_tab(buffer_temp);
	if (strchr(buffer_temp, ';')) {
		/*Multiple commands in a single line need to split*/
		/*Will need to do everything done below too!*/

		int command_number;

		char *cmd = strtok(buffer_temp, ";");
		while (cmd != NULL) {
			command_number = target_arr[target_pos].commands.command_count;
			strcpy(target_arr[target_pos].commands.list[command_number].com, cmd);
			target_arr[target_pos].commands.list[command_number].command_type = mult_cmd;
			trim_string(cmd);
			if (test_last_char(cmd, '&')) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = back_cmd;
			} else if (strchr(cmd, '<') && strchr(cmd, '>')) {
				/*Has both the directions*/
				target_arr[target_pos].commands.list[command_number].sp_command_type = redr_both_cmd;
			} else if (strchr(cmd, '<') || strchr(cmd, '>')) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = redr_cmd;
			} else if (test_cdir_cmd(cmd)) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = cdir_cmd;
			} else if (test_echo_cmd(cmd)) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = echo_cmd;
			} else {
				target_arr[target_pos].commands.list[command_number].sp_command_type = norm_cmd;
			}

			target_arr[target_pos].commands.command_count++;
			cmd = strtok(NULL, ";");
		}
		target_arr[target_pos].commands.list[command_number].command_type = mult_cmd_last;
	} else if (strchr(buffer_temp, '|')) {
		/*Piped commands*/

		int command_number;

		char *cmd = strtok(buffer_temp, "|");
		while (cmd != NULL) {
			command_number = target_arr[target_pos].commands.command_count;
			strcpy(target_arr[target_pos].commands.list[command_number].com, cmd);
			target_arr[target_pos].commands.list[command_number].command_type = pipe_cmd;
			trim_string(cmd);

			if (test_last_char(cmd, '&')) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = back_cmd;
			} else if (strchr(cmd, '<') || strchr(buffer_temp, '>')) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = redr_cmd;
			} else if (test_cdir_cmd(cmd)) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = cdir_cmd;
			} else if (test_echo_cmd(cmd)) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = echo_cmd;
			} else {
				target_arr[target_pos].commands.list[command_number].sp_command_type = norm_cmd;
			}

			target_arr[target_pos].commands.command_count++;
			cmd = strtok(NULL, "|");
		}
		target_arr[target_pos].commands.list[command_number].command_type = pipe_cmd_last;
	} else if (test_last_char(buffer_temp, '&')) {
		/*Background command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = back_cmd;
		target_arr[target_pos].commands.command_count++;
	} else if (strchr(buffer_temp, '<') && strchr(buffer_temp, '>')) {
		/*Has both the directions*/
		/*IO Redirection command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = redr_both_cmd;
		target_arr[target_pos].commands.command_count++;
	} else if (strchr(buffer_temp, '<') || strchr(buffer_temp, '>')) {
		/*IO Redirection command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = redr_cmd;
		target_arr[target_pos].commands.command_count++;
	} else if (test_cdir_cmd((buffer_temp))) {
		/*CD command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = cdir_cmd;
		target_arr[target_pos].commands.command_count++;
	} else if (test_echo_cmd((buffer_temp))) {
		/*ECHO command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = echo_cmd;
		target_arr[target_pos].commands.command_count++;
	} else {
		/*Normal command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = norm_cmd;
		target_arr[target_pos].commands.command_count++;
	}

}

void create_command_list(int pos) {
	int k = 0;
	int idx = find_target_idx(queue.targ_queue[pos]);
	for (int j = 0; j < target_arr[idx].commands.command_count; j++) {
		cmd_list[k++] = target_arr[idx].commands.list[j];
	}

	char *env_path = getenv(search_path_env);

	for (int i = 0; i < k; i++) {
		find_file(env_path, cmd_list[i].com);
	}

	if (target_arr[idx].target_type == infr_target) {
		char trg[STLEN];
		char dep[STLEN];
		if (target_arr[idx].inference_to == NULL) {
			printf("Single Inference\n");
			sprintf(trg, "%s", ui.target);
			sprintf(dep, "%s.%s", ui.target, target_arr[idx].inference_from);
		} else {
			printf("Double Inference\n");
			sprintf(trg, "%s.%s", ui.target, target_arr[idx].inference_to);
			sprintf(dep, "%s.%s", ui.target, target_arr[idx].inference_from);
		}

		/*Change stuff here for inference targets*/
		for (int i = 0; i < k; i++) {
			printf("Original command %-50s\n", cmd_list[i].com);
			char *pos;
			while ((pos = strstr(cmd_list[i].com, "$(TARGET)"))) {
				char buffer[STLEN];
				strncpy(buffer, cmd_list[i].com, pos - cmd_list[i].com);
				sprintf(buffer + (pos - cmd_list[i].com), "%s%s", trg, pos + strlen("$(TARGET)"));
				strcpy(cmd_list[i].com, buffer);
			}

			while ((pos = strstr(cmd_list[i].com, "$(DEP)"))) {
				char buffer[STLEN];
				strncpy(buffer, cmd_list[i].com, pos - cmd_list[i].com);
				sprintf(buffer + (pos - cmd_list[i].com), "%s%s", dep, pos + strlen("$(DEP)"));
				strcpy(cmd_list[i].com, buffer);

			}
			printf("Changed command %-50s\n", cmd_list[i].com);
		}
	}
}

void get_default_make() {
	/*Set default only if makefile not supplied*/
	struct stat buf;
	if (strcmp(ui.make_file_name, "\0") == 0) {

		if (stat("mymake1.mk", &buf) == 0) {
			/*mymake1.mk found, so set it as default*/
			strcpy(ui.make_file_name, "mymake1.mk");

		} else if (stat("mymake2.mk", &buf) == 0) {
			/*mymake2.mk found, so set it as default*/
			strcpy(ui.make_file_name, "mymake2.mk");
		} else if (stat("mymake3.mk", &buf) == 0) {
			/*mymak3.mk found, so set it as default*/
			strcpy(ui.make_file_name, "mymake3.mk");
		} else {
			/*Make file not found so Abort*/
			printf("No Makefiles found\n");
			exit(0);
		}
	} else {
		/*Test if supplied makefile exists*/
		if (stat(ui.make_file_name, &buf) < 0) {
			/*make_file_name found, so set it as default*/
			printf("Supplied makefile doesn't exist!\n");
			exit(0);
		}
	}
}

void kill_everything() {
	/*Kills everything in the same group*/
	kill(0, SIGKILL);
}

void handle_execution_error(int pos) {
	/*Execution error handler
	 * does stuff based on settings*/
	if (ui.force == false) {
		fprintf(stderr, "Error due to instruction %s\n", cmd_list[pos].com);
		kill_everything();
	} else {
		fprintf(stderr, "Error due to instruction %s\n", cmd_list[pos].com);
		/*Keep going*/
	}
}

void signal_handler(int signo) {
	if (ui.interrupt == true) {
		/*Blocking all SIGINTS*/
		printf("SIGINT caught\n");
	} else {
		kill_everything();
	}
}

void signal_alarm(int signo) {
	printf("Make Timed Out\n");
	kill_everything();
}

void execute_cdir_cmd(int start) {
	if (strlen(cmd_list[start].com) == 0) {
		/*Empty command, so just return!*/
		return;
	}
	char buffer_temp[STLEN];
	strcpy(buffer_temp, cmd_list[start].com);
	char *path = strtok(buffer_temp, " ");
	path = strtok(NULL, " ");
	print_dir();
	chdir(path);
	print_dir();
}

void execute_echo_cmd(int start) {
	if (strlen(cmd_list[start].com) == 0) {
		/*Empty command, so just return!*/
		return;
	}
	char buffer_temp[NUMELE];
	strcpy(buffer_temp, cmd_list[start].com);
	char *msg = strtok(buffer_temp, " ");
	msg = strtok(NULL, "\n");

	char *ptr;
	while ((ptr = strchr(msg, '\"'))) {
		*ptr = ' ';
	}

	while ((ptr = strchr(msg, '\''))) {
		*ptr = ' ';
	}

	trim_string(msg);
	printf("%s\n", msg);
}

void execute_back_cmd(int start) {
	print_dir();
	if (fork() == 0) {
		/*Executing program in child process*/
		char *exec_args[NUMELE];
		tokenize(cmd_list[start].com, exec_args);
		if (execv(exec_args[0], exec_args) == -1) {
			handle_execution_error(start);
			exit(0);
		}
	}
}

void execute_redr_both_cmd(int start) {
	char buffer_temp[NUMELE];
	strcpy(buffer_temp, cmd_list[start].com);
	int stdout = dup(STDOUT_FILENO);
	int stdin = dup(STDIN_FILENO);
	if (strchr(cmd_list[start].com, '>')) {
		/*Forward redirection command*/
		char *proc = strtok(buffer_temp, "<");
		char *file1 = strtok(NULL, ">");
		char *file2 = strtok(NULL, "\n");
		char *exec_args[100];

		trim_string(proc);
		trim_string(file1);
		trim_string(file2);
		remove_newline(file2);
		tokenize(proc, exec_args);
		printf("%s\n", proc);
		printf("%s\n", file1);
		printf("%s\n", file2);

		struct stat buf;
		if (stat(file1, &buf) < 0) {
			/*make_file_name found, so set it as default*/
			printf("Supplied file doesn't exist!\n");
			handle_execution_error(start);
		}
		close(STDIN_FILENO);
		int f1 = open(file1, O_RDONLY);

		close(STDOUT_FILENO);
		int f2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (fork() == 0) {
			if (execv(exec_args[0], exec_args) == -1) {
				perror("fork");
				handle_execution_error(start);
				exit(0);
			}
		} else {
			wait_all_children();
			close(f1);
			dup(stdin);
			close(f2);
			dup(stdout);
		}

	}
}

void execute_redr_cmd(int start) {
	char buffer_temp[NUMELE];
	strcpy(buffer_temp, cmd_list[start].com);
	int stdout = dup(STDOUT_FILENO);
	int stdin = dup(STDIN_FILENO);
	if (strchr(cmd_list[start].com, '>')) {
		/*Forward redirection command*/
		char *proc = strtok(buffer_temp, ">");
		char *file = strtok(NULL, "\n");
		char *exec_args[100];

		trim_string(proc);
		trim_string(file);
		remove_newline(file);
		tokenize(proc, exec_args);
		close(STDOUT_FILENO);
		int f1 = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (fork() == 0) {
			if (execv(exec_args[0], exec_args) == -1) {
				perror("fork");
				handle_execution_error(start);
				exit(0);
			}
		} else {
			wait_all_children();
			close(f1);
			dup(stdout);
		}

	} else {
		/*Backward redirection command*/
		char *proc = strtok(buffer_temp, "<");
		char *file = strtok(NULL, "\n");
		char *exec_args[100];

		trim_string(proc);
		trim_string(file);
		remove_newline(file);
		tokenize(proc, exec_args);
		close(STDIN_FILENO);

		struct stat buf;
		if (stat(file, &buf) < 0) {
			/*make_file_name found, so set it as default*/
			printf("Supplied file doesn't exist!\n");
			handle_execution_error(start);
		}
		int f1 = open(file, O_RDONLY);
		if (fork() == 0) {
			if (execv(exec_args[0], exec_args) == -1) {
				handle_execution_error(start);
				exit(0);
			}
		} else {
			wait_all_children();
			close(f1);
			dup(stdin);
		}
	}

	close(stdout);
	close(stdin);

	print_dir();
}

void execute_norm_cmd(int start) {
	print_dir();

	if (fork() == 0) {
		/*Executing program in child process*/
		char *exec_args[100];
		tokenize(cmd_list[start].com, exec_args);
		if (execv(exec_args[0], exec_args) == -1) {
			handle_execution_error(start);
			exit(0);
		}
	}
}

void execute_mult_cmd(int start, int count) {
	for (int i = start; i < start + count; i++) {
		if (cmd_list[i].sp_command_type == redr_cmd) {
			execute_redr_cmd(i);
			wait_all_children();
		} else if (cmd_list[i].sp_command_type == back_cmd) {
			execute_back_cmd(i);
			/*No waiting for background process*/
		} else if (cmd_list[i].sp_command_type == cdir_cmd) {
			/*Useless command probably does nothing*/
			execute_cdir_cmd(i);
			wait_all_children();
		} else if (cmd_list[i].sp_command_type == norm_cmd) {
			execute_norm_cmd(i);
			wait_all_children();
		} else {
			printf("This is not possible SOMETHING IS WRONG!%-50s%d\n", cmd_list[i].com, cmd_list[i].command_type);
		}
	}
}

void execute_pipe_cmd(int start, int count) {
	int num_pipe = count - 1;
	int pipe_lvl[num_pipe][2];
	for (int i = 0; i < num_pipe; i++) {
		pipe(pipe_lvl[i]);
	}

	for (int i = 0; i < count; i++) {
		if (i == 0) {
			/*The first process, so do not close STDIN!*/
			if (fork() == 0) {
				close(STDOUT_FILENO);
				dup(pipe_lvl[i][1]);
				/*Close everything after dup*/
				for (int i = 0; i < num_pipe; i++) {
					close(pipe_lvl[i][0]);
					close(pipe_lvl[i][1]);
				}

				/*Executing program in child process*/
				char *exec_args[100];
				tokenize(cmd_list[start + i].com, exec_args);
				if (execv(exec_args[0], exec_args) == -1) {
					handle_execution_error(start);
					exit(0);
				}
				exit(0);
			}
		} else if (i == count - 1) {
			/*The last process so do not close STDOUT*/
			if (fork() == 0) {
				close(STDIN_FILENO);
				dup(pipe_lvl[i - 1][0]);
				/*Close everything after dup*/
				for (int i = 0; i < num_pipe; i++) {
					close(pipe_lvl[i][0]);
					close(pipe_lvl[i][1]);
				}

				/*Executing program in child process*/
				char *exec_args[100];
				tokenize(cmd_list[start + i].com, exec_args);
				if (execv(exec_args[0], exec_args) == -1) {
					handle_execution_error(start);
					exit(0);
				}
				exit(0);
			}
		} else {
			/*Middle process, so close everything*/
			if (fork() == 0) {
				close(STDIN_FILENO);
				dup(pipe_lvl[i - 1][0]);
				close(STDOUT_FILENO);
				dup(pipe_lvl[i][1]);
				/*Close everything after dup*/
				for (int i = 0; i < num_pipe; i++) {
					close(pipe_lvl[i][0]);
					close(pipe_lvl[i][1]);
				}
				/*Executing program in child process*/
				char *exec_args[100];
				tokenize(cmd_list[start + i].com, exec_args);
				if (execv(exec_args[0], exec_args) == -1) {
					handle_execution_error(start);
					exit(0);
				}
				exit(0);
			}
		}
	}

	for (int i = 0; i < num_pipe; i++) {
		close(pipe_lvl[i][0]);
		close(pipe_lvl[i][1]);
	}
	wait_all_children();
}

bool test_requirements(int pos) {
	/*Testing if the requirements are satisfied or not
	 * Returns true if it is satisfied else false*/

	int idx = find_target_idx(queue.targ_queue[pos]);
	struct stat tar;
	struct stat dep;
	if (idx == -1) {
		/*Since it is a file, we don't have to do anything with it
		 * Target make rules only apply to Targets*/
//		printf("TEST : it is a file\n");
		return true;
	}

	if ((stat(queue.targ_queue[pos], &tar) == -1)) {
		/*Target doesn't exist, so we have to make it*/
//		printf("TEST : Target doesn't exist we have to compile\n");
		return false;
	}

	if (stat(queue.targ_queue[pos], &tar) == 0) {
		/*Test if dependecy exists!*/

		for (int i = 0; i < target_arr[idx].dependency_count; i++) {
			if ((stat(target_arr[idx].dependecies[i], &dep) == -1)) {
				/*Dependency doesn't exist, so we have to make it*/
				printf("TEST : Dependency doesn't exist we have to compile\n");
				return false;
			} else if (tar.st_mtime < dep.st_mtime) {
				/*Dependency was made after the Target*/
				printf("TEST : Dependency was made after the Target\n");
				return false;
			}
		}
	}
	printf("TEST : All's well\n");

	return false;

}

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Please pass correct input\n");
		return 0;
	}

	/*Default values initialized here */
	macro_arr = malloc(sizeof(struct macros) * NUMELE);
	target_arr = malloc(sizeof(struct targets) * NUMELE);
	strcpy(ui.make_file_name, "\0");
	strcpy(ui.target, "\0");
	ui.print = false;
	ui.force = false;
	ui.debug = false;
	ui.interrupt = false;
	ui.time = -1;
	counters.macros = 0;
	counters.targets = 0;
	counters.inferences = 0;
	counters.names = 0;
	counters.commands = 0;
	print_counter.macros = 0;
	print_counter.targets = 0;
	print_counter.inferences = 0;
	print_counter.names = 0;
	print_counter.commands = 0;
	queue.queue_end = 0;

	for (int i = 0; i < NUMELE; i++) {
		target_arr[i].dependency_count = 0;
		target_arr[i].commands.command_count = 0;
		target_arr[i].target_type = empty_target;
		for (int j = 0; j < NUMELE; j++) {
			target_arr[i].commands.list[NUMELE].command_type = empty_cmd;
			target_arr[i].commands.list[NUMELE].sp_command_type = empty_cmd;
		}
		queue.visited[i] = false;
	}

	char buffer_temp[STLEN];
	bool start = true; //Start ensures that we only match macros at the beginning
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			strcpy(ui.make_file_name, argv[++i]);
		} else if (strcmp(argv[i], "-p") == 0) {
			ui.print = true;
		} else if (strcmp(argv[i], "-k") == 0) {
			ui.force = true;
		} else if (strcmp(argv[i], "-d") == 0) {
			ui.debug = true;
		} else if (strcmp(argv[i], "-i") == 0) {
			ui.interrupt = true;
		} else if (strcmp(argv[i], "-t") == 0) {
			ui.time = atoi(argv[++i]);
		} else {
			strcpy(ui.target, argv[i]);
		}
	}
	getcwd(ui.cdir, sizeof(ui.cdir));
	get_default_make();
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
		perror("signal");
	}

	if (ui.interrupt == true) {
		sigset_t newmask, oldmask;
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGINT);
		if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
			perror("sigprocmask");
		}
	}

	if (ui.time != -1) {
		struct sigaction act;

		act.sa_handler = signal_alarm;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;

		sigaction(SIGALRM, &act, 0);
		alarm(ui.time);
	}

	FILE *file = fopen(ui.make_file_name, "r");
	while (fgets(buffer_temp, sizeof buffer_temp, file)) {
		char *pos;
		if ((pos = strchr(buffer_temp, '#'))) {
			/*Comments are present, so need to remove them*/
			*pos = '\0';
			if (strlen(buffer_temp) == 0) {
				/* If all the characters are comments, then eliminate the line.*/
				continue;
			}
		}

		if (strchr(buffer_temp, '=') && start == true) {
			/*Type macro*/
			get_macro(buffer_temp);
			print_counter.macros++;
		} else if (strchr(buffer_temp, ':')) {
			/*Type target*/
			start = false;
			get_target(buffer_temp, counters.targets);
		} else if (start == false) {
			/* The condition ensures that we count empty lines in targets not macros*/
			get_cmd(buffer_temp, counters.targets - 1);
			print_counter.commands++;
		}

	}

	rewind(file);
	if (ui.print == true) {
		printf("Macros : %d    Commands : %d    Targets : %d    Inferences : %d   \n", print_counter.macros, print_counter.commands, print_counter.targets, print_counter.inferences);
		while (fgets(buffer_temp, sizeof buffer_temp, file)) {
			char *pos;
			if ((pos = strchr(buffer_temp, '#'))) {
				/*Comments are present, so need to remove them*/
				*pos = '\0';
				if (strlen(buffer_temp) == 0) {
					/* If all the characters are comments, then eliminate the line.*/
					continue;
				}
			}
			if (strchr(buffer_temp, ':')) {
				printf("\n%s", buffer_temp);
			} else {
				printf("%s", buffer_temp);
			}

		}
		return 0;

	}

	/*Selecting first target if target is missing*/
	if (strcmp(ui.target, "\0") == 0) {
		/*Missing target , so use the first one*/
		strcpy(ui.target, target_arr[0].target_name);
	}

	/*Will need to modify here based on the flags!*/
	/*Trying to find the matching target*/
	int matched_target = find_target_idx(ui.target);

	if (matched_target == -1) {
		printf("Make ***No targets found\n");
		return (1);
	}

	if (ui.debug == true) {
		printf("%sDBG : Target Matched : %s\n", s, target_arr[matched_target].target_name);
	}
	dfs(ui.target);

	if (ui.debug == true) {
		printf("%sDBG : Target Queue : ", s);
		for (int i = 0; i < queue.queue_end; i++) {
			printf(" %s ", queue.targ_queue[i]);
		}
		printf("\n");
	}

	for (int i = 0; i < queue.queue_end; i++) {
		if (ui.debug == true) {
			printf("%s%sDBG : Making target %s\n", s, s, queue.targ_queue[i]);
		}
		if (test_requirements(i) == true) {
			/*All requirements are satisfied
			 * so skip it*/
			if (ui.debug == true) {
				printf("%s%sDBG : Already Satisfied\n", s, s);
			}
			continue;
		}

		cmd_list = malloc(sizeof(struct command_line) * NUMELE * NUMELE);
		for (int i = 0; i < NUMELE; i++) {
			cmd_list[i].command_type = empty_cmd;
		}

		create_command_list(i);

		int k = 0;

		/*Executing all of them from here*/
		while (cmd_list[k].command_type != empty_cmd) {
			if (strlen(cmd_list[k].com) == 0) {
				/*Ignoring empty commands*/
				k++;
			} else if (cmd_list[k].command_type == pipe_cmd) {
				int counter = 0;
				int start = k;
				char output[STLEN * NUMELE];
				if (ui.debug == true) {
					sprintf(output, "%s%s%sDBG : CMD : ", s, s, s);
				}
				while (cmd_list[k].command_type != pipe_cmd_last) {
					if (ui.debug == true) {
						sprintf(output, "%s %s |", output, cmd_list[k].com);
					}
					counter++;
					k++;
				}
				if (ui.debug == true) {
					sprintf(output, "%s %s ", output, cmd_list[k].com);
					printf(" %-150s  TYPE : %s\n", output, print_type(cmd_list[k].command_type));
				}
				counter++;
				k++;
				execute_pipe_cmd(start, counter);
				wait_all_children();
			} else if (cmd_list[k].command_type == mult_cmd) {
				int counter = 0;
				int start = k;
				char output[STLEN * NUMELE];
				if (ui.debug == true) {
					sprintf(output, "%s%s%sDBG : CMD : ", s, s, s);
				}
				while (cmd_list[k].command_type != mult_cmd_last) {
					if (ui.debug == true) {
						sprintf(output, "%s %s |", output, cmd_list[k].com);
					}
					counter++;
					k++;
				}
				if (ui.debug == true) {
					sprintf(output, "%s %s ", output, cmd_list[k].com);
					printf(" %-150s  TYPE : %s\n", output, print_type(cmd_list[k].command_type));
				}
				counter++;
				k++;
				execute_mult_cmd(start, counter);
				wait_all_children();

			} else if (cmd_list[k].command_type == redr_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				execute_redr_cmd(k);
				k++;
				wait_all_children();
			} else if (cmd_list[k].command_type == redr_both_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				execute_redr_both_cmd(k);
				k++;
				wait_all_children();
			} else if (cmd_list[k].command_type == back_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				execute_back_cmd(k);
				k++;
				/*No waitig for background process*/
			} else if (cmd_list[k].command_type == cdir_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				/*Useless command probably does nothing*/
				execute_cdir_cmd(k);
				k++;
				wait_all_children();
			} else if (cmd_list[k].command_type == echo_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				/*Useless command probably does nothing*/
				execute_echo_cmd(k);
				k++;
				wait_all_children();
			} else if (cmd_list[k].command_type == norm_cmd) {
				if (ui.debug == true) {
					printf("%s%s%sDBG : CMD : %-150s TYPE : %s\n", s, s, s, cmd_list[k].com, print_type(cmd_list[k].command_type));
				}
				printf("%s \n", cmd_list[k].com);
				execute_norm_cmd(k);
				k++;
				wait_all_children();
			} else {
				printf("This is not possible SOMETHING IS WRONG!%-50s%d\n", cmd_list[k].com, cmd_list[k].command_type);
				k++;
				wait_all_children();
			}

			/*Reset back to original directory after everything is done in a command*/
			chdir(ui.cdir);
		}

		if (ui.debug == true) {
			printf("%s%sDBG : Completed Making target %s\n", s, s, queue.targ_queue[i]);
		}
	}

}
