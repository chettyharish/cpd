all:
	gcc -o myprun myprun.c -Wall  -std=c99 -pedantic
	./myprun test.c -np 12

clean:
	rm myprun mypkill
