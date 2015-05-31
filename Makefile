CC=gcc
FLAG=-Wall -std=c99 -pedantic

all:
	$(CC) -o mymake mymake.c $(FLAG)
