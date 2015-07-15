FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops
all: 
	gcc -o splitter splitter.c $(FLAGS)
	gcc -o mysortold mysortold.c  $(FLAGS)
	gcc -o mysortbasic mysortbasic.c  $(FLAGS)
	gcc -o mysort mysort.c  $(FLAGS)
	gcc -o tester tester.c  $(FLAGS)
	gcc -o bin_to_ascii bin_to_ascii.c $(FLAGS)
	chmod 700 splitter 
	chmod 700 mysortold
	chmod 700 mysortbasic
	chmod 700 mysort 
	chmod 700 tester
	chmod 700 bin_to_ascii

first: mysort 
	time ./mysort /home/cop5570a/test0 1
second: mysort
	time ./mysort /home/cop5570a/test0 20 2
	
mysort: mysort.c
	gcc -o mysort mysort.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp

clean:
	rm -f bin_to_ascii splitter tester mysortold mysort mysortbasic temp*

