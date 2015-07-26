FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops
all: 
	gcc -o client_king client_king.c $(FLAGS)
	gcc -o server_king server_king.c $(FLAGS)
	
clean:
	rm -f client_king server_king mem_sort_server mem_sort_client bin_to_ascii splitter tester mysortold mysort mysortbasic temp*

