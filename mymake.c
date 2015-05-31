#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define STLEN 100
#define NUMELE 20

struct userinput {
	char make_file_name[STLEN];
	char target[STLEN];
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
	char com[NUMELE][STLEN];
	int command_count;
};

struct targets {
	char target_name[STLEN];
	int dependency_count;
	char dependecies[NUMELE][STLEN];
	struct commands commands;
};

struct macros {
	char macro[STLEN];
	char macro_replace[STLEN];
};

struct targets target_arr[NUMELE];
struct macros macro_arr[NUMELE];

void get_macro(char *buffer_temp, int macro_pos) {
	char *macro = strtok(buffer_temp, "=");
	char *macro_replace = strtok(NULL, "\n");

	strcpy(macro_arr[macro_pos].macro, macro);
	strcpy(macro_arr[macro_pos].macro_replace, macro_replace);
	printf("POS = %d\t\tMacro = %s\t\tMacro_Replace = %s\n", macro_pos,
			macro_arr[macro_pos].macro, macro_arr[macro_pos].macro_replace);
}

void get_target(char *buffer_temp, int target_pos) {
	char *target = strtok(buffer_temp, ":");
	strcpy(target_arr[target_pos].target_name, target);
	char *dependencies = strtok(NULL, " ");

	printf("\nTarget Name : %s\n", target_arr[target_pos].target_name);

	while (dependencies != NULL) {
		strcpy(
				target_arr[target_pos].dependecies[target_arr[target_pos].dependency_count++],
				dependencies);
		dependencies = strtok(NULL, " ");
	}

	for (int i = 0; i < target_arr[target_pos].dependency_count; ++i) {
		printf("Dependency Name : %s\n", target_arr[target_pos].dependecies[i]);
	}
}

void get_cmd(char *buffer_temp, int target_pos) {
	strcpy(target_arr[target_pos].commands.com[target_arr[target_pos].commands.command_count], buffer_temp);
	printf("TNUM = %d    CNUM = %d    CMD = %s", target_pos, target_arr[target_pos].commands.command_count,
			target_arr[target_pos].commands.com[target_arr[target_pos].commands.command_count]);
	target_arr[target_pos].commands.command_count++;

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

	for (int i = 0; i < NUMELE; ++i){
		target_arr[i].dependency_count = 0;
		target_arr[i].commands.command_count = 0;
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

//	printf("Make file = %s\n", ui.make_file_name);
//	printf("Target = %s\n", ui.target);
//	printf("print = %d\n", ui.print);
//	printf("force = %d\n", ui.force);
//	printf("debug = %d\n", ui.debug);
//	printf("interrupt = %d\n", ui.interrupt);
//	printf("time = %d\n", ui.time);

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
		} else if (start == false) {
			/* The condition ensures that we count empty lines in targets not macros*/
			get_cmd(buffer_temp, print_counters.targets - 1);
		}

	}

	return 0;
}
