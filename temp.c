
#include <stdio.h>

int main() {
	system("ssh program1 \"setenv MYID Hello; setenv MYID $MYID:Cat;\"");

}
