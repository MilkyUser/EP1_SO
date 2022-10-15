#include <math.h>
#include "scheduler.h"

int above(int pos){ return floor((pos - 1) / 2);}
int left(int pos){ return (2 * pos) + 1;}
int right(int pos){ return (2 * pos) + 2;}

void swap(program * a, program * b)
{
    program * temp = a;
    *a = *b;
    *b = *temp;
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

program * pop(program_queue * queue){
	program * result = &queue->root[0];
	queue->root[0] = queue->root[queue->size - 1];
	queue->size--;
	shift_down(queue, 0);
	return result;
}
