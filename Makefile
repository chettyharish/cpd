all:
	gcc -o myprun myprun.c -Wall -std=c99 -pedantic
	#gcc -o mypkill mypkill.c -Wall -std=c99 -pedantic
clean:
	rm myprun mypkill
