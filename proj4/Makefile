FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops -D_POSIX_C_SOURCE

all: 
	gcc -o tictactoe tictactoe.c $(FLAGS)
	
clean:
	rm -f tictactoe ttt*
