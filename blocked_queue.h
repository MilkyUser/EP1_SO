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

blocked_process_queue * initialize_blocked_process_queue();
void block_process(blocked_process_queue * BPQ, PCB * p);
void unblock_process(blocked_process_queue * BPQ, blocked_process * BP);
void decrement_remaining_time(blocked_process_queue * BPQ);
void test_drive_2();

