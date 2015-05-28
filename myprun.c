#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define printInfo 0

extern FILE *popen( const char *command, const char *modes);
extern int pclose(FILE *stream);

struct userinput {
	char mf_name[1000];
	int np;
	char p_name[1000];

} ui;

void printStatement(char * buffer_temp) {
	if ( printInfo == 1)
		printf("%s\n", buffer_temp);
}

void wait_all_children() {
	while (wait(NULL) > 0);
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

int main(int argc, char **argv) {
	int num_machines = 0, pid = -1;
	char buffer_temp[1000];
	char ssh_path[1000];
	char scp_path[1000];
	char *exec_args[100];

	strcpy(ui.mf_name, "machinefile");
	strcpy(ui.p_name, "NULL");
	ui.np = 1;

	if (argc == 1) {
		printf("Usage: myprun [-f machinefile]|[-np #] cmd\n");
		return 0;
	}

	for (int i = 1; i < argc; i++) {

		if (strcmp(argv[i], "-f") == 0) {
			strcpy(ui.mf_name, argv[++i]);
		} else if (strcmp(argv[i], "-np") == 0) {
			ui.np = atoi(argv[++i]);
		} else {
			strcpy(ui.p_name, argv[i]);
		}

	}

	if (strcmp(ui.p_name, "NULL") == 0) {
		printf("Please pass a correct C program to execute\n");
		return 0;
	}


	/* Getting all the paths of executables*/
	fgets(buffer_temp, sizeof(buffer_temp), popen("which ssh", "r"));
	sscanf(buffer_temp, "%s\n", ssh_path);
	fgets(buffer_temp, sizeof(buffer_temp), popen("which scp", "r"));
	sscanf(buffer_temp, "%s\n", scp_path);

	FILE *file = fopen(ui.mf_name, "r");
	if (file == NULL) {
		printf("Please pass a correct machinefile to execute\n");
		return 0;
	}

	printf("Launching 'myprun -f %s -np %d %s'\n", ui.mf_name, ui.np,
			ui.p_name);
	while (fgets(buffer_temp, sizeof buffer_temp, file)) {
		/*Detect the number of machines in the file*/
		++num_machines;
	}

	fseek(file, 0, SEEK_SET);
	char mac_list[num_machines][1000];
	for (int i = 0; i < num_machines; ++i) {
		/*Read the machines from machine file*/
		if (fscanf(file, "%s\n", mac_list[i]) == 0) {
			printf("Error reading %s\n", ui.mf_name);
		}

	}

	for (int i = 0; i < ui.np; i++) {
		/*Handles the directory creation*/
		if ((pid = fork()) == 0) {
			sprintf(buffer_temp, "%s %s -q mkdir temp%d", ssh_path,
					mac_list[i % num_machines], i);
			printStatement(buffer_temp);
			tokenize(buffer_temp, exec_args);
			if (execv(exec_args[0], exec_args) == -1) {
				printf("execv execution error!\n");
			}
		} else if (pid < 0) {
			printf("Child creation error!\n");
		}
	}

	wait_all_children();

	for (int i = 0; i < ui.np; i++) {
		/*Handles the directory removal*/
		if ((pid = fork()) == 0) {
			sprintf(buffer_temp, "%s -q %s %s:temp%d", scp_path,ui.p_name,
					mac_list[i % num_machines], i);
			printStatement(buffer_temp);
			tokenize(buffer_temp, exec_args);

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		} else if (pid < 0) {
			printf("Child creation error!\n");
		}
	}
	wait_all_children();

	for (int i = 0; i < ui.np; i++) {
		/*Handles the execution*/
		if ((pid = fork()) == 0) {
			sprintf(buffer_temp, "%s %s -q "
					"setenv PATH ${PATH}:/usr/sfw/bin "
					"&& setenv TSIZE %d "
					"&& setenv MYID %d "
					"&& cd temp%d "
					"&& gcc %s "
					"&& ./a.out ", ssh_path, mac_list[i % num_machines], ui.np,
					i, i, ui.p_name);
			printStatement(buffer_temp);
			tokenize(buffer_temp, exec_args);

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		} else if (pid < 0) {
			printf("Child creation error!\n");
		}

	}
	wait_all_children();

	for (int i = 0; i < ui.np; i++) {
		/*Handles the clearing after we are done with the program*/
		if ((pid = fork()) == 0) {
			sprintf(buffer_temp, "%s %s -q rm -rf temp%d > /dev/null "
					"&& unsetenv TSIZE "
					"&& unsetenv MYID "
					"&& setenv PATH ${PATH}:/usr/sfw/bin ", ssh_path,
					mac_list[i % num_machines], i);
			printStatement(buffer_temp);
			tokenize(buffer_temp, exec_args);

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		} else if (pid < 0) {
			printf("Child creation error!\n");
		}
	}
	wait_all_children();

	return 0;
}
