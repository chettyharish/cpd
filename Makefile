all:
	gcc -o seq seq.c -std=c99 -O3
	gcc -fopenmp -o omp omp.c -std=c99 -O3
	gcc -lpthread -o thread thread.c -std=c99 -O3
	gcc process.c -o process -std=c99 -O3

clean:
	rm -f seq omp thread process