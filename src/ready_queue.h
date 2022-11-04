/************************************
 EACH - USP | SISTEMAS DE INFORMAÇÃO
 EP1 - SO
 ANA POMARICO - 11208141
 BRUNO LEITE DE ANDRADE - 11369642
 DAYANE CARVALHO - 11319002
 VINÍCIUS M O DE MORAES - 11207748
 ************************************/

typedef struct PCB PCB;

typedef struct ready_queue
{
	// The PCB ** queue variable is an array of pointers to PCB structs
	PCB ** queue;
	int size;
} ready_queue;

int ready_queue_above(int pos);
int ready_queue_left(int pos);
int ready_queue_right(int pos);
void ready_queue_swap(PCB ** a, PCB ** b);
void ready_queue_shift_up(ready_queue * queue, int program_index);
void ready_queue_shift_down(ready_queue * queue, int program_index);
PCB * ready_queue_pop(ready_queue * queue);
void ready_queue_insert(ready_queue * queue, PCB * p);

