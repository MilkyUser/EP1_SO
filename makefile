all: scheduler

scheduler: scheduler.o scheduler_util.o ready_queue.o blocked_queue.o
	gcc -o scheduler scheduler.o scheduler_util.o ready_queue.o blocked_queue.o -std=c11 -lm
	make clean

scheduler.o: scheduler.c scheduler.h scheduler_util.h
	gcc -o scheduler.o scheduler.c -c -W -Wall -ansi -pedantic -std=c11

scheduler_util.o: scheduler_util.c scheduler_util.h 
	gcc -o scheduler_util.o scheduler_util.c -c -W -Wall -ansi -pedantic -std=c11

ready_queue.o: ready_queue.c scheduler.h ready_queue.h 
	gcc -o ready_queue.o ready_queue.c -c -W -Wall -ansi -pedantic -std=c11
	
blocked_queue.o: blocked_queue.c scheduler.h blocked_queue.h 
	gcc -o blocked_queue.o blocked_queue.c -c -W -Wall -ansi -pedantic -std=c11

clean:
	rm -rf *.o *~

