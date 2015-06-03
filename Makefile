CC=gcc
FLAG=-Wall -std=c99 -pedantic

all:
	$(CC) -o mymake mymake.c $(FLAG)
demo :
	$(CC) -o mymake mymake.c $(FLAG)
	./mymake -f new_test_makefile
clean:
	rm mymake
