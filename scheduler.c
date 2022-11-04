#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "scheduler.h"
#include "ready_queue.h"
#include "blocked_queue.h"
#include "scheduler_util.h"

// This function consumes all ready processes until all processes have 0 credit
void consume_ready_queue(ready_queue * r_queue, blocked_process_queue * BPQ)
{
	while (r_queue->size > 0)
	{
		ready_process_buffer * newly_ready = decrement_remaining_time(BPQ);
		for (int i=0; i<newly_ready->size; i++)
		{
			ready_queue_insert(r_queue, newly_ready->ready_process_buffer[i]);
			printf("\tINSERT %s\n", newly_ready->ready_process_buffer[i]->name); 
		}
		free(newly_ready->ready_process_buffer);
		free(newly_ready);
		PCB * running = ready_queue_pop(r_queue);
		running->current_state = RUNNING;
		printf("Executando %s\n", running->name);

		int c;
		for (
			c = running->current_command;
			running->current_command < global_quantum + c && running->current_command < running->command_count;
			running->current_command++
			)
		{	
			if (running->credit <= 0)
			{
				block_process(BPQ, running, 0);
				break;
			}
			running->credit--;
			if (strcmp(running->commands[running->current_command], "E/S")==0)
			{
				running->current_command++;
				printf("E/S iniciada em %s\n", running->name);
				block_process(BPQ, running, 2);
				break;
			}
			if (running->commands[running->current_command][0] == 'X')
			{
				global_reg.x = strtol(&running->commands[running->current_command][2], NULL, 10);
			}
			else if (running->commands[running->current_command][0] == 'Y')
			{
				global_reg.y = strtol(&running->commands[running->current_command][2], NULL, 10);
			}
			else if (strcmp(running->commands[running->current_command], "SAIDA")==0)
			{
				// TODO
			}
			
		}
		
		if (running->current_state != BLOCKED && running->credit > 0)
		{
			ready_queue_insert(r_queue, running);
		}
		running->current_command-c == 1 ? 
		printf("Interrompendo %s após 1 instrução\n", running->name) :
		printf("Interrompendo %s após %d instruções\n", running->name, running->current_command-c);
		
		printf("\n\n----------\n\n");
    
		for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
		{
			printf("PROGRAM: %s, STATE: %d, CURRENT_COM: %d, COM_COUNT: %d, CREDIT %d\n", BP->blocked_process->name, BP->blocked_process->current_state, BP->blocked_process->current_command, BP->blocked_process->command_count, BP->blocked_process->credit);
		}
    	printf("\n\n----------\n\n");

	}
}

int main(int argc, char * argv[])
{
	// Open config files
	FILE * quantum_file = fopen("quantum.txt", "r");
    FILE * priorities_file = fopen("prioridades.txt", "r");
    
    // Set global variables
    global_reg = (registers) {NULL, NULL};
    fscanf(quantum_file, "%d", &global_quantum);
    fclose(quantum_file);
    
    if (global_quantum > 999)
    {
    	// Let's not take any chances...
        fprintf(stderr, "Quantum too big!");
        exit(1);
    }
    
    FILE ** program_files = calloc(1000, sizeof(FILE*));
    int program_count = get_program_files(program_files, argv[1]);
    PCB * process_table = malloc(sizeof(PCB)*program_count);

	// Loop through all "program_file_pointers" and initialize the programs in the Process Table
	for (int current_program = 0; current_program < program_count; current_program++)
    {
        process_table[current_program].name = malloc(64);
        fgets(process_table[current_program].name, 64, program_files[current_program]);
        process_table[current_program].name[strcspn(process_table[current_program].name, "\r\n")] = '\0';

        if (!feof(priorities_file))
        {
        	// Max priority accepted: 255
            unsigned char * value = malloc(1);
            fscanf(priorities_file, "%d", value);
            process_table[current_program].priority = *value;
            process_table[current_program].credit = *value;
        }
        else
        {
            printf(stderr, "ERRO: ARQUIVO DE PRIORIDADES CONTEM MENOS PRIORIDADES QUE A"
                            " QUANTIDADE DE PROGRAMAS NA PASTA:\n %s\n", argv[1]);
            exit(1);
        }
		
		// First the commands in the program_file are counted and only then the commands are set in the PCB
        int command_count = 0;
        while (!feof(program_files[current_program]))
        {
            if (getc(program_files[current_program]) == '\n')
            {
                command_count++;
            }
        }

        // As it's stated before: let's not take any chances
        if (command_count > 10000) 
        {
        	printf(stderr, "ERRO: Programa %s contem muitos comandos!\n", process_table[current_program].name);
            exit(1);
        }
		
        process_table[current_program].command_count = command_count;
        process_table[current_program].current_command = 0;
        
        // Restart the file pointer and sets the commands array
        fseek(program_files[current_program], strlen(process_table[current_program].name), SEEK_SET);
        process_table[current_program].commands = malloc(16*command_count);
        for (int current_command=0; current_command<command_count;current_command++)
        {
            char * command = malloc(16);
            fscanf(program_files[current_program], "%s", command);
            process_table[current_program].commands[current_command] = command;
        }
        fclose(program_files[current_program]);
        
        process_table[current_program].current_state = READY;
        process_table[current_program].reg = (registers) {NULL, NULL};
        
    } 
    // Finishes initialization of Process Table
    
    // Initializes the priority queue of ready processes
    ready_queue * r_queue = calloc(1, sizeof(ready_queue));
    r_queue->size = 0;
	r_queue->queue = calloc(1000, sizeof(PCB));
    for (int i=0; i<program_count; i++)
    {	
    	ready_queue_insert(r_queue, &process_table[i]);
    }
   	
    // TODO: main loop	
    
    //test_drive_2();
    //return 0;
    blocked_process_queue * BPQ = initialize_blocked_process_queue();
    consume_ready_queue(r_queue, BPQ);
    
    printf("\n\n----------\n\n");
    
    for (int i=0; i<r_queue->size; i++)
    {
    	printf("PROGRAM: %s, STATE: %d, CURRENT_COM: %d, COM_COUNT: %d, CREDIT %d\n", r_queue->queue[i]->name, r_queue->queue[i]->current_state, r_queue->queue[i]->current_command, r_queue->queue[i]->command_count, r_queue->queue[i]->credit);
    }
    
    printf("\n\n----------\n\n");
    
    for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
    {
    	printf("PROGRAM: %s, STATE: %d, CURRENT_COM: %d, COM_COUNT: %d, CREDIT %d\n", BP->blocked_process->name, BP->blocked_process->current_state, BP->blocked_process->current_command, BP->blocked_process->command_count, BP->blocked_process->credit);
    }
    
    printf("\n\n----------\n\n");
    
    for (int i=0; i<program_count; i++)
    {
    	printf("PROGRAM: %s, STATE: %d, CURRENT_COM: %d, COM_COUNT: %d, CREDIT %d\n", process_table[i].name, process_table[i].current_state, process_table[i].current_command, process_table[i].command_count, process_table[i].credit);
    }

	FILE* logf;
    char* log_file_name = malloc(12 + log10(global_quantum));
    log10(global_quantum) < 1 ?
    sprintf(log_file_name, "log0%d.txt", global_quantum) :
    sprintf(log_file_name, "log%d.txt", global_quantum);
    logf = fopen(log_file_name, "a");
    fprintf(logf, "QUANTUM: %d\n", global_quantum);
    fclose(logf);
    
    return 0;
}
