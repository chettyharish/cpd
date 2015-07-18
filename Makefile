FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops
all: 
	gcc -o mem_sort_server mem_sort_server.c $(FLAGS)
	gcc -o mem_sort_client mem_sort_client.c $(FLAGS)
	gcc -o splitter splitter.c $(FLAGS)
	gcc -o mysortold mysortold.c  $(FLAGS)
	gcc -o mysortbasic mysortbasic.c  $(FLAGS)
	gcc -o mysort mysort.c  $(FLAGS)
	gcc -o tester tester.c  $(FLAGS)
	gcc -o bin_to_ascii bin_to_ascii.c $(FLAGS)
	gcc -o sort_server sort_server.c $(FLAGS)
	gcc -o sort_client sort_client.c $(FLAGS)
	chmod 700 splitter 
	chmod 700 mysortold
	chmod 700 sort_server 
	chmod 700 sort_client
	chmod 700 mysortbasic
	chmod 700 mysort 
	chmod 700 tester
	chmod 700 bin_to_ascii
	
	
	
clean:
	rm -f bin_to_ascii splitter tester mysortold mysort mysortbasic temp*

