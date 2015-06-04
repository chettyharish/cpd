#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define STLEN 200
#define NUMELE 50
#define empty_target -2
#define empty_cmd -1
#define norm_cmd 0
#define pipe_cmd 1
#define redr_cmd 2
#define cdir_cmd 3
#define back_cmd 4
#define mult_cmd 5
#define pipe_cmd_last 6
#define mult_cmd_last 7
#define norm_target 0
#define infr_target 1

struct userinput {
	char make_file_name[STLEN];
	char target[STLEN];
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

struct stack {
	char st[NUMELE][STLEN];
	bool visited[NUMELE];
	int stack_top;
} stack;

struct targets *target_arr;
struct macros *macro_arr;
struct command_line *cmd_list;

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

	if (file_name[0] == '/') {
		/*Absolute path found dont change anything*/
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
			printf("%s MATCH FOUND %s \n", target, target_arr[i].target_name);
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
	if (idx == -1) {
		/*Test if it is a file in the current directory*/
		struct stat buf;
		if (stat(target, &buf) == 0) {
			strcpy(stack.st[stack.stack_top++], target);
		} else {
			printf("Missing Target : %s\n", target);
		}
		return;
	}

	if (stack.visited[idx] == false) {
		stack.visited[idx] = true;
		strcpy(stack.st[stack.stack_top++], target_arr[idx].target_name);

		for (int i = 0; i < target_arr[idx].dependency_count; i++) {
			dfs(target_arr[idx].dependecies[i]);
		}

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

void get_target(char *buffer_temp, int target_pos) {

	counters.targets++;
	char *target = strtok(buffer_temp, ":");
	trim_string(target);
	strcpy(target_arr[target_pos].target_name, target);
	char *dependencies = strtok(NULL, " ");

	printf("\nTarget Name : %s\n", target_arr[target_pos].target_name);

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
	printf("Target type = %d\n", target_arr[target_pos].target_type);
	printf("From = %s\n", target_arr[target_pos].inference_from);
	printf("To = %s\n", target_arr[target_pos].inference_to);
	for (int i = 0; i < target_arr[target_pos].dependency_count; ++i) {
		printf("Dependency Name : %s\n", target_arr[target_pos].dependecies[i]);
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
			} else if (strchr(cmd, '<') || strchr(buffer_temp, '>')) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = redr_cmd;
			} else if (strstr(cmd, "cd ")) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = cdir_cmd;
			} else {
				target_arr[target_pos].commands.list[command_number].sp_command_type = norm_cmd;
			}

			target_arr[target_pos].commands.command_count++;
			cmd = strtok(NULL, ";");
		}
		target_arr[target_pos].commands.list[command_number].command_type = mult_cmd_last;

		printf("Multiple Command\n");
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
			} else if (strstr(cmd, "cd ")) {
				target_arr[target_pos].commands.list[command_number].sp_command_type = cdir_cmd;
			} else {
				target_arr[target_pos].commands.list[command_number].sp_command_type = norm_cmd;
			}

			target_arr[target_pos].commands.command_count++;
			cmd = strtok(NULL, "|");
		}
		target_arr[target_pos].commands.list[command_number].command_type = pipe_cmd_last;

		printf("Piped command\n");
	} else if (test_last_char(buffer_temp, '&')) {
		/*Background command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = back_cmd;
		target_arr[target_pos].commands.command_count++;

		printf("Background command\n");
	} else if (strchr(buffer_temp, '<') || strchr(buffer_temp, '>')) {
		/*IO Redirection command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = redr_cmd;
		target_arr[target_pos].commands.command_count++;
		printf("IO Redirection command\n");
	} else if (strstr(buffer_temp, "cd ")) {
		/*CD command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = cdir_cmd;
		target_arr[target_pos].commands.command_count++;

		printf("CD command\n");
	} else {
		/*Normal command*/

		int command_number = target_arr[target_pos].commands.command_count;
		strcpy(target_arr[target_pos].commands.list[command_number].com, buffer_temp);

		target_arr[target_pos].commands.list[command_number].command_type = norm_cmd;
		target_arr[target_pos].commands.command_count++;

		printf("Normal command\n");
	}

}

void create_command_list() {
	int k = 0;
	for (int i = stack.stack_top - 1; i >= 0; i--) {
		int idx = find_target_idx(stack.st[i]);
		if (idx == -1) {
			/*It's a file*/
			continue;
		}
		for (int j = 0; j < target_arr[i].commands.command_count; j++) {
			cmd_list[k++] = target_arr[i].commands.list[j];
		}
	}

	char *env_path = getenv("PATH");

	for (int i = 0; i < k; i++) {
		printf("Original command %-50s\n", cmd_list[i].com);
		if (find_file(env_path, cmd_list[i].com) == 1) {
			printf("File not found\n");
		} else {
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

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Please pass correct input\n");
		return 0;
	}

	/*Default values initialized here */
	char *temp_exec_args[NUMELE];
	macro_arr = malloc(sizeof(struct macros) * NUMELE);
	target_arr = malloc(sizeof(struct targets) * NUMELE);
	cmd_list = malloc(sizeof(struct command_line) * NUMELE * NUMELE);
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
	stack.stack_top = 0;

	for (int i = 0; i < NUMELE; i++) {
		target_arr[i].dependency_count = 0;
		target_arr[i].commands.command_count = 0;
		target_arr[i].target_type = empty_target;
		cmd_list[i].command_type = empty_cmd;
		for (int j = 0; j < NUMELE; j++) {
			target_arr[i].commands.list[NUMELE].command_type = empty_cmd;
			target_arr[i].commands.list[NUMELE].sp_command_type = empty_cmd;
		}
		stack.visited[i] = false;
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

	get_default_make();

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

	printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

	printf("Macros : %d    Commands : %d    Targets : %d    Inferences : %d    Names : %d\n", print_counter.macros, print_counter.commands, print_counter.targets, print_counter.inferences,
			print_counter.names);

	for (int i = 0; i < counters.targets; i++) {
		printf("\n\n");
		printf("Target %s \n", target_arr[i].target_name);
		printf("Dependencies : ");
		for (int j = 0; j < target_arr[i].dependency_count; j++) {
			printf(" %s ", target_arr[i].dependecies[j]);
		}
		printf("\n");

		printf("Commands : \n");
		for (int j = 0; j < target_arr[i].commands.command_count; j++) {
			printf("CMD =  %-100sProtocol = %-3d SP_PROTOCOL %d\n", target_arr[i].commands.list[j].com, target_arr[i].commands.list[j].command_type, target_arr[i].commands.list[j].sp_command_type);
		}
		printf("\n");

	}

	/*Selecting first target if target is missing*/
	if (strcmp(ui.target, "\0") == 0) {
		/*Missing target , so use the first one*/
		strcpy(ui.target, target_arr[0].target_name);
	}

	printf("Make file = %s\n", ui.make_file_name);
	printf("Target = %s\n", ui.target);
	printf("print = %d\n", ui.print);
	printf("force = %d\n", ui.force);
	printf("debug = %d\n", ui.debug);
	printf("interrupt = %d\n", ui.interrupt);
	printf("time = %d\n", ui.time);

	/*Will need to modify here based on the flags!*/
	/*Trying to find the matching target*/
	int matched_target = find_target_idx(ui.target);

	if (matched_target == -1) {
		printf("Make ***No targets found\n");
		return (1);
	}

	dfs(ui.target);
	for (int i = 0; i < stack.stack_top; i++) {
		printf("STACK %d %s\n", i, stack.st[i]);
	}
	printf("%d\n", stack.stack_top);
	create_command_list();


	int k = 0 ;
	while(cmd_list[k].command_type != empty_cmd){
		tokenize(cmd_list[k].com , temp_exec_args);
		int counter = 0;
		while(1){
			printf("TEMP : %-20s" , temp_exec_args[counter]);
			if(temp_exec_args[counter] == '\0')
				break;
			counter++;
		}
		printf("\n");

		k++;
	}



	return 0;
}
