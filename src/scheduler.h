typedef struct ready_queue ready_queue;
int global_quantum;
int total_interruptions;
int program_count;
int quantum_iter_count;
int total_command_count;

typedef enum
{
	READY,
	RUNNING,
	BLOCKED,
	FINISHED
} process_state;

typedef struct registers
{
    int x;
    int y;
} registers;

registers * global_reg;

typedef struct PCB
{
    char * name;
    char priority;
    char ** commands;
    int command_count;
    char credit;
    int current_command;
    process_state current_state;
    registers * reg;
} PCB;

PCB * process_table;

