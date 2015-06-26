all:
	gcc -o seq seq.c -std=c99 -O3
	gcc -fopenmp -o omp omp.c -std=c99 -O3
	gcc -lpthread -o thread thread.c -std=c99 -O3
	gcc -o process process.c  -std=c99 -O3

seq:
	gcc -o seq seq.c -std=c99 -O3

omp:
	setenv OMP_NUM_THREADS 32 
	gcc -fopenmp -o omp omp.c -std=c99 -O3
	
thread:
	gcc -lpthread -o thread thread.c -std=c99 -O3

process:
	gcc -o process process.c  -std=c99 -O3
	
clean:
	rm -f seq omp thread process