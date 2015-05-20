#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct userinput {
	char mf_name[1000];
	char p_name[1000];

} ui;

int main(int argc, char *argv[]) {
	strcpy(ui.mf_name, "machinefile");
	strcpy(ui.p_name, "NULL");

	if (argc == 1) {
		printf("Please pass the correct arguments\n");
		return 0;
	}

	for (int i = 1; i < argc; i++) {

		if (strcmp(argv[i], "-f") == 0) {
			strcpy(ui.mf_name, argv[++i]);
		} else {
			strcpy(ui.p_name, argv[i]);
		}

	}

	if (strcmp(ui.p_name, "NULL") == 0) {
		printf("Please pass a C program to execute\n");
		return 0;
	}

	printf("%s\n", ui.mf_name);
	printf("%s\n", ui.p_name);
	return 0;
}
