typedef struct registers
{
    int x;
    int y;
} registers;

typedef struct program
{
    char * name;
    char priority;
    char ** commands;
    int command_count;
    registers reg;
} program;

typedef struct program_queue
{
	program * root;
	int size;
} program_queue;

typedef struct blocked_process
{
    program * process;
    char remaining_time;
    struct blocked_process * next_process;
} blocked_process;

typedef struct blocked_queue
{
    blocked_process * head;
} blocked_queue;

int above(int pos);
int left(int pos);
int right(int pos);
void swap(program * a, program * b);
void shift_up(program_queue * queue, int program_index);
void shift_down(program_queue * queue, int program_index);
program * pop(program_queue * queue);
void insert(program_queue * queue, program * p);
void test_drive();
