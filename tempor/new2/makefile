

test1: test2 test3
	echo $@ > /dev/null
test2: test3
	echo $@ > /dev/null
test3: temp.c
	echo $@ > /dev/null
	gcc -o test3 temp.c
