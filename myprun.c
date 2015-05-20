#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct userinput {
	char mf_name[1000];
	int np;
	char p_name[1000];

} ui;

int main(int argc, char *argv[]) {
	int num_machines = 0;
	char buffer[1000];

	strcpy(ui.mf_name, "machinefile");
	strcpy(ui.p_name, "NULL");
	ui.np = 1;

	if (argc == 1) {
		printf("Please pass the correct arguments\n");
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
		printf("Please pass a C program to execute\n");
		return 0;
	}

//	printf("%s\n", ui.mf_name);
//	printf("%s\n", ui.p_name);
//	printf("%d\n", ui.np);

	FILE *file = fopen(ui.mf_name, "r");
	if (file == NULL) {
		perror("Error");
		exit(1);
	}

	while (fgets(buffer, sizeof buffer, file)) {
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
		int counter = 0;
		char buffer_temp[1000];
		char *exec_args[100];

		/*Handles the directory removal*/
		if ((fork()) == 0) {
			sprintf(buffer_temp, "/usr/bin/ssh %s -q mkdir temp%d > /dev/null",
					mac_list[i % num_machines], i);
			printf("%s\n", buffer_temp);

			char *token = strtok(buffer_temp, " ");
			while (token != NULL) {
				exec_args[counter] = token;
				counter++;
				token = strtok(NULL, " ");
			}
			exec_args[counter] = NULL;

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		}
	}
	while (wait(NULL) > 0);


	for (int i = 0; i < ui.np; i++) {
		int counter = 0;
		char buffer_temp[1000];
		char *exec_args[100];

		/*Handles the directory removal*/
		if ((fork()) == 0) {
			sprintf(buffer_temp, "/usr/bin/scp -q %s %s:temp%d",
					ui.p_name,mac_list[i % num_machines], i);
			printf("%s\n", buffer_temp);

			char *token = strtok(buffer_temp, " ");
			while (token != NULL) {
				exec_args[counter] = token;
				counter++;
				token = strtok(NULL, " ");
			}
			exec_args[counter] = NULL;

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		}
	}
	while (wait(NULL) > 0);

	for (int i = 0; i < ui.np; i++) {
		int counter = 0;
		char buffer_temp[1000];
		char *exec_args[100];

		/*Handles the execution*/
		if ((fork()) == 0) {
			sprintf(buffer_temp, "/usr/bin/ssh %s -q "
					"setenv PATH ${PATH}:/usr/sfw/bin "
					"&& setenv TSIZE %d "
					"&& setenv MYID %d "
					"&& cd temp%d "
					"&& gcc %s "
					"&& ./a.out ", mac_list[i % num_machines], ui.np, i, i,
					ui.p_name);
			printf("%s\n", buffer_temp);

			char *token = strtok(buffer_temp, " ");
			while (token != NULL) {
				exec_args[counter] = token;
				counter++;
				token = strtok(NULL, " ");
			}
			exec_args[counter] = NULL;

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		}

	}
	while (wait(NULL) > 0);

	for (int i = 0; i < ui.np; i++) {
		int counter = 0;
		char buffer_temp[1000];
		char *exec_args[100];

		/*Handles the directory removal*/
		if ((fork()) == 0) {
			sprintf(buffer_temp, "/usr/bin/ssh %s -q rm -rf temp%d > /dev/null",
					mac_list[i % num_machines], i);
			printf("%s\n", buffer_temp);

			char *token = strtok(buffer_temp, " ");
			while (token != NULL) {
				exec_args[counter] = token;
				counter++;
				token = strtok(NULL, " ");
			}
			exec_args[counter] = NULL;

			if (execv(exec_args[0], exec_args) == -1) {
				printf("Command execution error!\n");
			}
		}
	}
	while (wait(NULL) > 0);

	return 0;
}
