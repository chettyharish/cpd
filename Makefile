all: seq omp thread process
	@echo 'Done Compiling'

simpledemo: seq omp thread process
	@echo 'Testing with 1000X1000 Dimensions'
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

boundarydemo: seq omp thread process
	@echo 'Testing with prime numbers for unequal work'
	@echo ''
	time ./seq 1427 1553 > testseq
	time ./omp 1427 1553 > testomp
	time ./thread 1427 1553 > testthread
	time ./process 1427 1553 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt
	@echo ''
	@echo 'Testing for a single col of data'
	@echo ''
	time ./seq 1 20 > testseq
	time ./omp 1 20 > testomp
	time ./thread 1 20 > testthread
	time ./process 1 20 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt
	@echo ''
	@echo 'Testing for a single row of data'
	@echo ''
	time ./seq 20 1 > testseq
	time ./omp 20 1 > testomp
	time ./thread 20 1 > testthread
	time ./process 20 1 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt
	@echo ''
	@echo 'Testing for a single element'
	@echo ''
	time ./seq 1 1 > testseq
	time ./omp 1 1 > testomp
	time ./thread 1 1 > testthread
	time ./process 1 1 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt
	@echo ''
	@echo 'Testing for a small matrix'
	@echo ''
	time ./seq 10 10 > testseq
	time ./omp 10 10 > testomp
	time ./thread 10 10 > testthread
	time ./process 10 10 > testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt

debugdemo: seq omp thread process
	time ./seq 100 100 -d 20 > testseq
	time ./omp 100 100 -d 20 > testomp
	time ./thread 100 100 -d 20> testthread
	time ./process 100 100 -d 20> testprocess
	diff testseq testomp
	diff testseq testthread
	diff testseq testprocess
	diff final_worldseq.txt final_worldomp.txt
	diff final_worldseq.txt final_worldthread.txt
	diff final_worldseq.txt final_worldprocess.txt	
	
seq: seq.c
	gcc -o seq seq.c -std=c99 -O3 -lm -pedantic

omp: omp.c
	gcc -o omp omp.c -std=c99 -O3 -lm -pedantic -fopenmp
	
thread: thread.c
	gcc -o thread thread.c -std=c99 -O3 -lm -pedantic -pthread

process: process.c
	gcc -o process process.c  -std=c99 -O3 -lm -pedantic
	
clean:
	rm -f seq omp thread process
	rm -f seq_debug omp_debug thread_debug process_debug
	rm -f testseq testomp testthread testprocess
	rm -f final_worldomp.txt final_worldseq.txt final_worldprocess.txt final_worldthread.txt

