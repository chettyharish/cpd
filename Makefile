all:
	gcc -o splitter splitter.c -O3 -std=c99
	gcc -o mysort mysort.c -O3 -std=c99 -lm -pedantic -pthread -D_POSIX_SOURCE

demo:all
	time ./mysort op