#include <stdlib.h>
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

void block_process(blocked_process_queue * BPQ, PCB * p)
{
	p->current_state = BLOCKED;
	blocked_process * b = malloc(sizeof(blocked_process));
	if (BPQ->HEAD != NULL)
	{
		BPQ->HEAD->previous = b;
	}
	b->previous = NULL;
	b->next = BPQ->HEAD;
	b->remaining_time = 2;
	b->blocked_process = p;
	BPQ->HEAD = b;
	BPQ->size++;
}

void unblock_process(blocked_process_queue * BPQ, blocked_process * BP)
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
	ready_queue_insert(r_queue, BP->blocked_process);
	free(BP);
}

void decrement_remaining_time(blocked_process_queue * BPQ)
{
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		if (BP->remaining_time<=0)
		{
			unblock_process(BPQ, BP);
			continue;
		}
		BP->remaining_time--;
	}
}


void test_drive_2()
{
	PCB p1 = (PCB) {"TESTE 1", 10, NULL, 0, 10, 0, 3, READY, NULL};
	PCB p2 = (PCB) {"TESTE 2", 8, NULL, 0, 10, 0, 3, READY, NULL};
	PCB p3 = (PCB) {"TESTE 3", 12, NULL, 0, 10, 0, 3, READY, NULL};
	blocked_process_queue * BPQ = initialize_blocked_process_queue();
	block_process(BPQ, &p3);
	block_process(BPQ, &p1);
	decrement_remaining_time(BPQ);
	block_process(BPQ, &p2);
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		printf("%s | %d\n", BP->blocked_process->name, BP->remaining_time);
	}
	printf("\n\n");
	unblock_process(BPQ, BPQ->HEAD->next->next);
	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
	{
		printf("%s | %d\n", BP->blocked_process->name, BP->remaining_time);
	}
}
