#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "ready_queue.h"
#include "scheduler.h"

int ready_queue_above(int pos){ return floor((pos - 1) / 2);}
int ready_queue_left(int pos){ return (2 * pos) + 1;}
int ready_queue_right(int pos){ return (2 * pos) + 2;}

void ready_queue_swap(PCB ** a, PCB ** b)
{
    PCB * temp = *a;
    *a = *b;
    *b = temp;
}

void ready_queue_shift_up(ready_queue * queue, int PCB_index)
{
    int PCB_index_above = ready_queue_above(PCB_index);
    
    while(queue->queue[PCB_index_above]->priority < queue->queue[PCB_index]->priority)
    {   
        ready_queue_swap(&queue->queue[PCB_index], &queue->queue[PCB_index_above]);
        PCB_index = PCB_index_above;
        PCB_index_above = ready_queue_above(PCB_index);
    }
}

void ready_queue_shift_down(ready_queue * queue, int PCB_index)
{
	int max_index = PCB_index;
	int left_PCB_index = ready_queue_left(PCB_index);
	int right_PCB_index = ready_queue_right(PCB_index);
	if (left_PCB_index <= queue->size &&
		queue->queue[left_PCB_index]->priority > queue->queue[max_index]->priority)
	{
		max_index = left_PCB_index;
	}
	if (right_PCB_index <= queue->size &&
		queue->queue[right_PCB_index]->priority > queue->queue[max_index]->priority)
	{
		max_index = right_PCB_index;
	}
	if (PCB_index != max_index)
	{
		ready_queue_swap(&queue->queue[PCB_index], &queue->queue[max_index]);
		ready_queue_shift_down(queue, max_index);
	}
}

void ready_queue_insert(ready_queue * queue, PCB * p)
{
	queue->size++;
	queue->queue[queue->size-1] = p;
	ready_queue_shift_up(queue, queue->size-1);
}

PCB * ready_queue_pop(ready_queue * queue)
{
	PCB * result = queue->queue[0];
	queue->queue[0] = queue->queue[queue->size-1];
	queue->size--;
	ready_queue_shift_down(queue, 0);
	return result;
}

void test_drive()
{

	PCB * p1 = calloc(1, sizeof(PCB));
	PCB * p2 = calloc(1, sizeof(PCB));
	PCB * p3 = calloc(1, sizeof(PCB));
	PCB * p4 = calloc(1, sizeof(PCB));
	PCB * p5 = calloc(1, sizeof(PCB));
	PCB * p6 = calloc(1, sizeof(PCB));
	PCB * p7 = calloc(1, sizeof(PCB));
	PCB * p8 = calloc(1, sizeof(PCB));
	PCB * p9 = calloc(1, sizeof(PCB));
	PCB * p10 = calloc(1, sizeof(PCB));
	p1->name = "TESTE 1";
	p2->name = "TESTE 2";
	p3->name = "TESTE 3";
	p4->name = "TESTE 4";
	p5->name = "TESTE 5";
	p6->name = "TESTE 6";
	p7->name = "TESTE 7";
	p8->name = "TESTE 8";
	p9->name = "TESTE 9";
	p10->name = "TESTE 10";
	p1->priority = 1;
	p2->priority = 3;
	p3->priority = 2;
	p4->priority = 10;
	p5->priority = 4;
	p6->priority = 6;
	p7->priority = 12;
	p8->priority = 8;
	p9->priority = 8;
	p10->priority = 4;
	
	ready_queue * queue = malloc(sizeof(ready_queue));
	queue->queue = calloc(10, sizeof(PCB));
	queue->size = 0;
	 
	ready_queue_insert(queue, p1);
	ready_queue_insert(queue, p2);
	ready_queue_insert(queue, p3);
	ready_queue_insert(queue, p4);
	ready_queue_insert(queue, p5);
	ready_queue_insert(queue, p6);
	ready_queue_insert(queue, p7);
	ready_queue_insert(queue, p8);
	ready_queue_insert(queue, p9);
	ready_queue_insert(queue, p10);
	
	printf("p1 pointer: %p\n", p1);
	
	for (int i=0; i<queue->size; i++)
	{
		printf("%s | PRIORIDADE %d\n", queue->queue[i]->name, queue->queue[i]->priority);
	}
	
	printf("\n\n");
	
	for (int i=0; i<10; i++)
	{
		PCB p = *ready_queue_pop(queue);
		printf("%s | PRIORIDADE %d\n", p.name, p.priority);
	}

}

