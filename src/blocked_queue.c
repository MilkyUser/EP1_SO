#include <stdlib.h>
#include <stdio.h>
#include "scheduler.h"
#include "blocked_queue.h"
#include "ready_queue.h"

// Linked list blocked queue wrapper struct 
blocked_process_queue * initialize_blocked_process_queue()
{
	blocked_process_queue * BPQ = malloc(sizeof(blocked_process_queue));
	BPQ->size = 0;
	BPQ->HEAD = NULL;
	return BPQ;
}

void block_process(blocked_process_queue * BPQ, PCB * p, int wait_time)
{
	total_interruptions++;
	p->current_state = BLOCKED;
	p->reg->x = global_reg->x;
	p->reg->y = global_reg->y;
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
	// Restoring registers:
	RPB->ready_process_buffer[RPB->size] = BP->blocked_process;
	RPB->size++;
	free(BP);
}

// Flushes unblocked process in the ready_process_buffer returned
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
