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
}  program;

typedef struct program_queue
{
    program * root;
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
void shift_up(program * p, program * program_list, int program_index);
void shift_down(program * p, program * program_list, int program_index);
void peek(program * p, program * program_list, int program_index);
void pop(program * p, program * program_list, int program_index);