FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops
all: 
	gcc -o mem_sort_server mem_sort_server.c $(FLAGS)
	gcc -o mem_sort_client mem_sort_client.c $(FLAGS)
	gcc -o bin_to_ascii bin_to_ascii.c $(FLAGS)
	
	
clean:
	rm -f mem_sort_server mem_sort_client bin_to_ascii splitter tester mysortold mysort mysortbasic temp*

