all: scheduler

scheduler: main.o priority_queue.o
	gcc -o scheduler main.o priority_queue.o -std=c11 -lm
	make clean

main.o: main.c scheduler.h
	gcc -o main.o main.c -c -W -Wall -ansi -pedantic -std=c11
	
priority_queue.o: priority_queue.c scheduler.h
	gcc -o priority_queue.o priority_queue.c -c -W -Wall -ansi -pedantic -std=c11

clean:
	rm -rf *.o *~
