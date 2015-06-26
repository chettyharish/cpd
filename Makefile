all:
	gcc -o seq seq.c -std=c99 -O3 -pedantic
	gcc -fopenmp -o omp omp.c -std=c99 -O3 -pedantic
	gcc -pthread -o thread thread.c -std=c99 -O3 -lm -pedantic
	gcc -o process process.c  -std=c99 -O3 -pedantic -lm

seq:
	gcc -o seq seq.c -std=c99 -O3

omp:
	gcc -fopenmp -o omp omp.c -std=c99 -O3
	
thread:
	gcc -lpthread -o thread thread.c -std=c99 -O3

process:
	gcc -o process process.c  -std=c99 -O3
	
clean:
	rm -f seq omp thread process
	rm -f  testomp testseq testthread testprocess
	rm -f final_worldomp.txt final_worldseq.txt final_worldprocess.txt final_worldthread.txt

