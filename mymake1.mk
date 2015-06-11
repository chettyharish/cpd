CC=/usr/bin/gcc
CPP=/usr/bin/g++ 

all : pipetest multiline redrdest bothredrtest cdecho
	echo "Done"

pipetest:
	/bin/cat Makefile | /bin/grep CC
	/bin/cat Makefile | /bin/grep CC | /bin/grep =
	/bin/cat mymake.c | /bin/grep char | /bin/grep STLEN | /bin/grep NUMELE| /bin/grep targ_queue
	/bin/cat mymake.c | /bin/grep char | /bin/grep STLEN | /usr/bin/nl -v0 | /bin/grep NUMELE 
multiline:
	cd tempor;cd new ; /bin/cat makefile ;

backtest:
	/bin/sleep 100 &
	/bin/sleep 50

backtest2:
	/bin/sleep 100 &

alarmtest:
	/bin/sleep 100

redrtest:
	/bin/cat Makefile > temp.txt
	/bin/grep FLAG < Makefile
bothredrtest:
	/usr/bin/sort < Makefile > temp.txt

echotest:
	echo "Hello World"
	echo "$(CC) $(CPP) $(CC)  $(CPP)"
	echo Hello World
	echo $CPP $CC



#Inference rules
.c.o:
	echo $<
	echo $@
	$(CC) -o $@ $<

.cpp:
	echo $<
	$(CPP) -o $@ $<

#For testing cycles and ordering
first : second third fourth
	echo $@
second : fifth sixth
	echo $@
third : first
	echo $@
fourth: second
	echo $@
fifth:  fourth third
	echo $@
sixth:  first
	echo $@

test1: test2 test3
	echo $@
test2: test3
	echo $@
test3: temp.c
	echo $@


#For testing absolute and dynamic commands
mixtest:
	/bin/cat Makefile
	cat Makefile
	/bin/ls Makefile
	ls Makefile




#For testing errors

pipeerror:
	/bin/cat Makefile | /bin/grep all

cderror: 
	cd what/where

errortest:
	/bin/wrongfile
errortest2:
	/bin/whatthehell | /bin/cat file | /nofile
errortest3: unknowntarg
	echo "Hello"



clean:
	rm -f temp.o
	rm -f temp2
