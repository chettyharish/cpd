#include <stdio.h>
#include <stdlib.h>

int main() {

	char *argv[3];
	argv[0] = "/usr/bin/cat";
	argv[1] = "Makefile";
	argv[2] = NULL;

	printf("%d\t\t%s\n",0,argv[0]);
	printf("%d\t\t%s\n",1,argv[1]);
	printf("%d\t\t%s\n",2,argv[2]);
	execv(argv[0], argv);
}
