
#include <stdio.h>

int main() {
	char buf[1000];
	fgets(buf, sizeof(buf), popen("which ssh", "r"));
	sscanf(buf,"%s\n",buf);
	printf("PATH IS %s", buf);

}
