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
    
    while(queue->queue[PCB_index_above]->credit < queue->queue[PCB_index]->credit)
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
		queue->queue[left_PCB_index]->credit > queue->queue[max_index]->credit)
	{
		max_index = left_PCB_index;
	}
	if (right_PCB_index <= queue->size &&
		queue->queue[right_PCB_index]->credit > queue->queue[max_index]->credit)
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

