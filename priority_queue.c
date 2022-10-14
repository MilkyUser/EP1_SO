#include <math.h>
#include "scheduler.h"

int above(int pos) return floor((pos - 1) / 2);
int left(int pos) return (2 * pos) + 1;
int right(int pos) return (2 * pos) + 2;

void swap(program * a, program * b)
{
    program * temp = a;
    *a = *b;
    *b = *temp;
}

void shift_up(program * p, program * program_list, int program_index)
{
    program_index_above = above(program_index)
    while(p->priority > program_list[program_index_above].priority)
    {
        swap(program_list, p, program_list[program_index_above])
        program_index = program_index_above;
    }
}