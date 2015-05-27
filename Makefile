CC=gcc
FLAG=-Wall -std=gnu99 -pedantic

all:
	$(CC) -o myprun myprun.c $(FLAG)
	$(CC) -o mypkill mypkill.c $(FLAG)
	
demo: myprun mypkill
	./myprun -f machinefile -np 12 test.c

myprun : myprun.c
	$(CC) -o myprun myprun.c $(FLAG)
	
mypkill: mypkill.c
	$(CC) -o mypkill mypkill.c $(FLAG)
	

clean:
	rm -f myprun mypkill myprun.o mypkill.o
