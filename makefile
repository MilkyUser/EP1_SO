all: scheduler

scheduler: scheduler.o ready_queue.o blocked_queue.o
	gcc -o scheduler scheduler.o ready_queue.o blocked_queue.o -std=c11 -lm
	make clean

main.o: scheduler.c scheduler.h
	gcc -o main.o scheduler.c -c -W -Wall -ansi -pedantic -std=c11
	
ready_queue.o: ready_queue.c scheduler.h ready_queue.h 
	gcc -o ready_queue.o ready_queue.c -c -W -Wall -ansi -pedantic -std=c11
	
blocked_queue.o: blocked_queue.c scheduler.h blocked_queue.h 
	gcc -o blocked_queue.o blocked_queue.c -c -W -Wall -ansi -pedantic -std=c11

clean:
	rm -rf *.o *~

