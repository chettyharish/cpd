#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

struct userinput {
	char make_file_name[1000];
	char target[1000];
	bool print;
	bool force;
	bool debug;
	bool interrupt;
	int time;
} ui;

struct print_counters {
	int macros;
	int targets;
	int inferences;
	int names;
	int commands;
} print_counters;

struct commands {
	char com[1000][1000];
};

struct targets {
	char target_name[1000];
	char dependecies[1000][1000];
	struct commands commands;
};

struct macros {
	char macro[1000];
	char macro_replace[1000];
};

struct targets target_arr[1000];
struct macros macro_arr[1000];

void get_macro(char *buffer_temp, int macro_pos) {
}

void get_target(char *buffer_temp, int target_pos) {
}

void get_cmd(char *buffer_temp, int target_pos, int cmd_pos) {
	sprintf(target_arr[target_pos].commands.com[cmd_pos], "%s", buffer_temp);
	printf("TNUM = %d    CNUM = %d    CMD = %s\n", target_pos, cmd_pos,
			target_arr[target_pos].commands.com[cmd_pos]);
}

void get_default_make() {
	/*Set default only if makefile not supplied*/
	struct stat buf;
	if (strcmp(ui.make_file_name, "NULL") == 0) {

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

void tokenize(char *buffer_temp, char *exec_args[]) {
	int counter = 0;
	char *token = strtok(buffer_temp, " ");
	while (token != NULL) {
		exec_args[counter] = token;
		printf("<%s>", exec_args[counter]);
		counter++;
		token = strtok(NULL, " ");
	}
	printf("\n");
	exec_args[counter] = NULL;
}

int main(int argc, char **argv) {

	if (argc == 1) {
		printf("Please pass correct input\n");
		return 0;
	}

	/*Default values initialized here */
	strcpy(ui.make_file_name, "NULL");
	strcpy(ui.target, "NULL");
	ui.print = false;
	ui.force = false;
	ui.debug = false;
	ui.interrupt = false;
	ui.time = -1;
	print_counters.macros = 0;
	print_counters.targets = 0;
	print_counters.inferences = 0;
	print_counters.names = 0;
	print_counters.commands = 0;

	char buffer_temp[1000];
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

//	printf("Make file = %s\n", ui.make_file_name);
//	printf("Target = %s\n", ui.target);
//	printf("print = %d\n", ui.print);
//	printf("force = %d\n", ui.force);
//	printf("debug = %d\n", ui.debug);
//	printf("interrupt = %d\n", ui.interrupt);
//	printf("time = %d\n", ui.time);

	int command_counter = 0;
	FILE *file = fopen(ui.make_file_name, "r");
	while (fgets(buffer_temp, sizeof buffer_temp, file)) {
//		printf("%s", buffer_temp);

		if (strchr(buffer_temp, '=') && start == true) {
			/*Type macro*/
			get_macro(buffer_temp, print_counters.macros);
			print_counters.macros++;
		} else if (strchr(buffer_temp, ':')) {
			/*Type target*/
			start = false;
			get_target(buffer_temp, print_counters.targets);
			print_counters.targets++;
			command_counter = 0;
		} else if (start == false) {
			/* The condition ensures that we count empty lines in targets not macros*/
			get_cmd(buffer_temp, print_counters.targets - 1, command_counter);
			command_counter++;
		}

		printf("\n");
	}

	return 0;
}
