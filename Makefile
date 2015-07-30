FLAGS= -march=native -Ofast -std=c99 -lm -pedantic -pthread -fopenmp -funroll-loops -mavx2
all: 
	gcc -o client_king client_king.c $(FLAGS)
	gcc -o server_king server_king.c $(FLAGS)
	gcc -o answertester answertester.c $(FLAGS)
	
clean:
	rm -f client_king server_king  answertester
