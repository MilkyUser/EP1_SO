#include <stdlib.h>
#include <stdio.h>
#include "scheduler.h"
#include "blocked_queue.h"
#include "ready_queue.h"

blocked_process_queue * initialize_blocked_process_queue()
{
	blocked_process_queue * BPQ = malloc(sizeof(blocked_process_queue));
	BPQ->size = 0;
	BPQ->HEAD = NULL;
	return BPQ;
}

void block_process(blocked_process_queue * BPQ, PCB * p, int wait_time)
{
	p->current_state = BLOCKED;
	p->reg = global_reg;
	blocked_process * b = malloc(sizeof(blocked_process));
	if (BPQ->HEAD != NULL)
	{
		BPQ->HEAD->previous = b;
	}
	b->previous = NULL;
	b->next = BPQ->HEAD;
	b->remaining_time = wait_time;
	b->blocked_process = p;
	BPQ->HEAD = b;
	BPQ->size++;
}

void unblock_process(ready_process_buffer * RPB, blocked_process_queue * BPQ, blocked_process * BP)
{
	if (BP->previous == NULL)
	{
		BPQ->HEAD = BP->next;
		if (BP->next != NULL)
		{
			BP->next->previous = NULL;
		}
	}
	else
	{
		BP->previous->next = BP->next;
		if (BP->next != NULL)
		{
			BP->next->previous = BP->previous;
		}
	}
	BP->blocked_process->current_state = READY;
	if (RPB->size >= RPB->capacity)
	{
		RPB->capacity *= 2;
		RPB = realloc(RPB, RPB->capacity);
		if (RPB == NULL)
		{
			printf(stderr, "ERRO: MEMÓRIA INSUFICIENTE\n");
            exit(1);
		}
	}
	RPB->ready_process_buffer[RPB->size] = BP->blocked_process;
	RPB->size++;
	free(BP);
}

ready_process_buffer * decrement_remaining_time(blocked_process_queue * BPQ)
{
	ready_process_buffer * RPB = malloc(sizeof(ready_process_buffer));
	RPB->ready_process_buffer = calloc(10, sizeof(PCB*));
	RPB->size = 0;
	RPB->capacity = 10;
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		BP->remaining_time--;
		if (BP->remaining_time<=0 && BP->blocked_process->credit > 0)
		{
			unblock_process(RPB, BPQ, BP);
		}
	}
	return RPB;
}

void test_drive_2()
{
	PCB p1 = (PCB) {"TESTE 1", 10, NULL, 0, 10, 0, 3, READY, NULL};
	PCB p2 = (PCB) {"TESTE 2", 8, NULL, 0, 8, 0, 3, READY, NULL};
	PCB p3 = (PCB) {"TESTE 3", 12, NULL, 0, 12, 0, 3, READY, NULL};
	blocked_process_queue * BPQ = initialize_blocked_process_queue();
	block_process(BPQ, &p3, 2);
	block_process(BPQ, &p1, 2);	
	block_process(BPQ, &p2, 0);
	printf("%s | %p\n", p1.name, &p1);
	printf("%s | %p\n", p2.name, &p2);
	printf("%s | %p\n", p3.name, &p3);
	printf("\n\n-----------\n\n");
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		printf("%s REMAINING_TIME: %d | %p\n", BP->blocked_process->name, BP->remaining_time, BP->blocked_process);
	}
	printf("\n\n-----------\n\n");
	ready_process_buffer * RPB = decrement_remaining_time(BPQ);
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		printf("%s REMAINING_TIME: %d | %p\n", BP->blocked_process->name, BP->remaining_time, BP->blocked_process);
	}
	printf("\n\n-----------\n\n");
	for (int i=0; i<RPB->size; i++)
	{
		printf("%s | %p\n", RPB->ready_process_buffer[i]->name, RPB->ready_process_buffer[i]);
	}
	printf("\n\n-----------\n\n");
	RPB = decrement_remaining_time(BPQ);
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		printf("%s REMAINING_TIME: %d | %p\n", BP->blocked_process->name, BP->remaining_time, BP->blocked_process);
	}
	printf("\n\n-----------\n\n");
	for (int i=0; i<RPB->size; i++)
	{
		printf("%s | %p\n", RPB->ready_process_buffer[i]->name, RPB->ready_process_buffer[i]);
	}
	
	RPB->ready_process_buffer = calloc(1, sizeof(PCB*));
	RPB->size = 0;
	RPB->capacity = 1;
	
}
