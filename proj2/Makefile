OS = $(shell uname -s)
CC=gcc
FLAG=-Wall -std=c99 -pedantic 
ifeq ($(OS),Linux)
FLAG+=-D_POSIX_SOURCE
endif
ifeq ($(OS),SunOS)
FLAG+=-D_XOPEN_SOURCE=600
endif
all:
	$(CC) -o mymake mymake.c $(FLAG)

flagcheck:
	echo $(CC)
	echo $(FLAG)
	echo $(OS)
clean:
	rm mymake
