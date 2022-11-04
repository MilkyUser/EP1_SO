typedef struct PCB PCB;

typedef struct blocked_process
{
    PCB * blocked_process;
    char remaining_time;
    struct blocked_process * previous;
    struct blocked_process * next;
} blocked_process;

typedef struct blocked_process_queue
{
    blocked_process * HEAD;
	int size;
} blocked_process_queue;

typedef struct ready_process_buffer
{
	// The PCB ** ready_process_buffer variable is an array of pointers to PCB structs
	PCB ** ready_process_buffer;
	int size;
	int capacity;
} ready_process_buffer;

blocked_process_queue * initialize_blocked_process_queue();
void block_process(blocked_process_queue * BPQ, PCB * p, int wait_time);
void unblock_process(ready_process_buffer * RPB, blocked_process_queue * BPQ, blocked_process * BP);
ready_process_buffer * decrement_remaining_time(blocked_process_queue * BPQ);
void test_drive_2();

