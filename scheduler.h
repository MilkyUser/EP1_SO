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
