typedef enum
{
	READY,
	RUNNING,
	BLOCKED,
} process_state;

typedef struct registers
{
    int x;
    int y;
} registers;

typedef struct PCB
{
    char * name;
    char priority;
    char ** commands;
    int command_count;
    char credit;
    int current_command;
    char quantum;
    process_state current_state;
    registers reg;
} PCB;

void free_process(PCB * p);
void test_drive();

