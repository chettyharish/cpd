all:
	gcc -o myprun myprun.c -Wall -std=c99 -pedantic
	./myprun test.c -f mfile -np 10

clean:
	rm myprun mypkill
