all:
	gcc -o seq seq.c -std=c99 -O3 -pedantic
	gcc -fopenmp -o omp omp.c -std=c99 -O3 -pedantic
	gcc -pthread -o thread thread.c -std=c99 -O3 -lm -pedantic
	gcc -o process process.c  -std=c99 -O3 -pedantic -lm

primedemo: seq omp thread process
	time ./seq 1093 1277 > testseq
	time ./omp 1093 1277 > testomp
	time ./thread 1093 1277 > testthread
	time ./process 1093 1277 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt

simpledemo: seq omp thread process
	time ./seq 1000 1000 > testseq
	time ./omp 1000 1000 > testomp
	time ./thread 1000 1000 > testthread
	time ./process 1000 1000 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt
	
debugdemo:
	gcc -o seq_debug seq_debug.c -std=c99 -O3 -lm -pedantic
	gcc -o omp_debug omp_debug.c -std=c99 -O3 -lm -pedantic -fopenmp 
	gcc -o thread_debug thread_debug.c -std=c99 -O3 -lm -pedantic -pthread
	gcc -o process_debug process_debug.c  -std=c99 -O3 -lm -pedantic 
	time ./seq_debug 100 100 > testseq
	time ./omp_debug 100 100 > testomp
	time ./thread_debug 100 100 > testthread
	time ./process_debug 100 100 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt	
	
seq: seq.c
	gcc -o seq seq.c -std=c99 -O3 -pedantic

omp: omp.c
	gcc -fopenmp -o omp omp.c -std=c99 -O3 -pedantic
	
thread: thread.c
	gcc -pthread -o thread thread.c -std=c99 -O3 -lm -pedantic

process: process.c
	gcc -o process process.c  -std=c99 -O3 -pedantic -lm
	
clean:
	rm -f seq omp thread process
	rm -f seq_debug omp_debug thread_debug process_debug
	rm -f testseq testomp testthread testprocess
	rm -f final_worldomp.txt final_worldseq.txt final_worldprocess.txt final_worldthread.txt

