#include <stdio.h>
#include <stdlib.h>

int main() {

	char *argv[3];
	argv[0] = "/usr/bin/cat";
	argv[1] = "Makefile";
	argv[2] = NULL;
	execv(argv[0], argv);
}
