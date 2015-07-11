all:
	gcc -o splitter splitter.c -O3 -std=c99 -lm -pedantic -pthread -fopenmp
	gcc -o mysort mysort.c -O3 -std=c99 -lm -pedantic -pthread -fopenmp

first: all
	time ./mysort op 1
second: all
	time ./mysort op 2
third: all
	time ./mysort op 3
four: all
	time ./mysort op 4
