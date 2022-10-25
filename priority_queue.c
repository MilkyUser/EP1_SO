#include <math.h>
#include <stdlib.h>
#include "scheduler.h"

int above(int pos){ return floor((pos - 1) / 2);}
int left(int pos){ return (2 * pos) + 1;}
int right(int pos){ return (2 * pos) + 2;}

void swap(program * a, program * b)
{
    program temp = *a;
    *a = *b;
    *b = temp;
}

void shift_up(program_queue * queue, int program_index)
{
    int program_index_above = above(program_index);
    while(queue->root[program_index].priority > queue->root[program_index_above].priority)
    {
        swap(&queue->root[program_index], &queue->root[program_index_above]);
        program_index = program_index_above;
    }
}

void shift_down(program_queue * queue, int program_index)
{
	int max_index = program_index;
	int left_program_index = left(program_index);
	int right_program_index = right(program_index);
	if (left_program_index <= queue->size &&
		queue->root[left_program_index].priority > queue->root[max_index].priority)
	{
		max_index = left(program_index);
	}
	if (right_program_index <= queue->size &&
		queue->root[right_program_index].priority > queue->root[max_index].priority)
	{
		max_index = right(program_index);
	}
	if (program_index != max_index)
	{
		swap(&queue->root[program_index], &queue->root[max_index]);
		shift_down(queue, max_index);
	}
}

void insert(program_queue * queue, program * p)
{
	queue->size++;
	queue->root[queue->size-1] = *p;
	shift_up(queue, queue->size-1);
	printf("%d\n", p->priority);
}

program * pop(program_queue * queue)
{
	program * result = &queue->root[0];
	queue->root[0] = queue->root[queue->size - 1];
	queue->size--;
	shift_down(queue, 0);
	return result;
}

void test_drive()
{
	program_queue * queue = malloc(sizeof(program_queue));
	queue->size = 0;
	queue->root = calloc(1000, sizeof(program));
	program * prog1, * prog2, * prog3, * prog4, * prog5, * prog6, * prog7, * prog8, * prog9;
	prog1 = malloc(sizeof(program));
	prog2 = malloc(sizeof(program));
	prog3 = malloc(sizeof(program));
	prog4 = malloc(sizeof(program));
	prog5 = malloc(sizeof(program));
	prog6 = malloc(sizeof(program));
	prog7 = malloc(sizeof(program)); 
	prog8 = malloc(sizeof(program));
	prog9 = malloc(sizeof(program));
	prog1->priority = 45;
	prog2->priority = 20;
	prog3->priority = 14;
	prog4->priority = 12;
	prog5->priority = 31;
	prog6->priority = 7;
	prog7->priority = 11; 
	prog8->priority = 13;
	prog9->priority = 7;
	insert(queue, prog1);
	insert(queue, prog2);
	insert(queue, prog3);
	insert(queue, prog4);
	insert(queue, prog5);
	insert(queue, prog6);
	insert(queue, prog7);
	insert(queue, prog8);
	insert(queue, prog9);
	
	for (int i = 0; i<queue->size; i++)
	{
		printf("%d ", queue->root[i].priority);
	}
	
	pop(queue);
	printf("\n");
	
	for (int i = 0; i<queue->size; i++)
	{
		printf("%d ", queue->root[i].priority);
	}
	
	printf("\nsize: %d\n", queue->size);
	
}
