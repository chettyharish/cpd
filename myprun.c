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


	for (int i = 0; i < num_machines; ++i) {
		/*scp the executable if the file doesnt exist on the remote server*/
		sprintf(buffer, "ssh %s -q test -e %s > /dev/null", mac_list[i], ui.p_name);
		printf("%s\n", buffer);
		int ret = system(buffer);
		if (ret != 0) {
			/*The file does not exist, so scp it*/
			sprintf(buffer, "scp %s %s: > /dev/null", ui.p_name, mac_list[i]);
			printf("%s\n", buffer);
			system(buffer);

		}
	}


	printf("\n\n");

	/*Run the jobs on the machines*/
	for (int i = 0; i < ui.np; i++) {
		sprintf(buffer, "ssh %s -q "
				"\"setenv PATH $PATH:/usr/sfw/bin "
				"&& setenv TSIZE %d "
				"&& setenv MYID %d "
				"&& gcc %s && ./a.out\"", mac_list[i % num_machines], ui.np, i,
				ui.p_name);
		printf("%s\n", buffer);
		system(buffer);
	}

	printf("\n\n");

	for (int i = 0; i < num_machines; ++i) {
		/*Remove the executable which was copied to the remote server!*/
		sprintf(buffer, "ssh %s -q test -e %s > /dev/null", mac_list[i], ui.p_name);
		printf("%s\n", buffer);
		int ret = system(buffer);
		if (ret == 0) {
			/*The file exists, so remove it*/
			sprintf(buffer, "ssh %s -q rm %s > /dev/null", mac_list[i], ui.p_name);
			printf("%s\n", buffer);
			system(buffer);

		}
	}
	/*system("scp test.c linprog1:");
	 system("scp test.c linprog2:");
	 system("scp test.c linprog3:");
	 system("scp test.c linprog4:");
	 system("ssh linprog1 \"setenv TSIZE 5 && setenv MYID 3 && gcc -o test test.c && ./test\"");
	 system("ssh linprog2 \"setenv TSIZE 5 && setenv MYID 3 && gcc -o test test.c && ./test\"");
	 system("ssh linprog3 \"setenv TSIZE 5 && setenv MYID 3 && gcc -o test test.c && ./test\"");
	 system("ssh linprog4 \"setenv TSIZE 5 && setenv MYID 3 && gcc -o test test.c && ./test\"");

	 getchar();
	 system("ssh linprog1 \"unsetenv TSIZE && unsetenv MYID && rm test.c\"");
	 */
	return 0;
}
