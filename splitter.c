#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

int main(int argc , char **argv){
	int num = atoi(argv[1]);
	long int *data = malloc(sizeof(long int) * num);
	long int *data2 = malloc(sizeof(long int) * num);
	FILE *file = fopen("/home/cop5570a/test0", "r");
	FILE *file2 = fopen("op", "w+");
	for(int i = 0 ; i < num ; i++){
		if(fread(&data[i], sizeof(long int), 1, file) == -1)
			perror("fread");
		if(fwrite(&data[i], sizeof(long int), 1, file2) == -1)
			perror("fwrite");
	}

//	fseek(file2 , 0,SEEK_SET);
//	for(int i = 0 ; i < num ; i++){
//		if(fread(&data2[i], sizeof(long int), 1, file2) == -1)
//			perror("fread");
//		if(data[i] != data2[i])
//			perror("Mismatch");
//	}

}
