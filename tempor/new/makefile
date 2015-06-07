first : second third fourth
	echo first > /dev/null
second : fifth sixth
	echo second > /dev/null
third : first
	echo third > /dev/null
fourth: second
	echo fourth > /dev/null
fifth:  fourth third
	echo fifth > /dev/null
sixth:  first
	echo $@ > /dev/null

