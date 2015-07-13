all: 
	gcc -o splitter splitter.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp
	gcc -o mysortold mysortold.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp
	gcc -o mysort mysort.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp
	gcc -o tester tester.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp
	gcc -o bin_to_ascii bin_to_ascii.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp

first: mysort 
	time ./mysort /home/cop5570a/test0 1
second: mysort
	time ./mysort /home/cop5570a/test0 20 2
	
mysort: mysort.c
	gcc -o mysort mysort.c -Ofast -std=c99 -lm -pedantic -pthread -fopenmp

clean:
	rm -f bin_to_ascii splitter tester mysortold mysort temp_lvl* temp*

