/************************************
 EACH - USP | SISTEMAS DE INFORMAÇÃO
 EP1 - SO
 ANA POMARICO - 11208141
 BRUNO LEITE DE ANDRADE - 11369642
 DAYANE CARVALHO - 11319002
 VINÍCIUS M O DE MORAES - 11207748
 ************************************/

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
		quantum_iter_count++;
		ready_process_buffer * newly_ready = decrement_remaining_time(BPQ);
		
		for (int i=0; i<newly_ready->size; i++)
		{
			ready_queue_insert(r_queue, newly_ready->ready_process_buffer[i]);
		}
		
		free(newly_ready->ready_process_buffer);
		free(newly_ready);
		
		PCB * running = ready_queue_pop(r_queue);
		global_reg->x = running->reg->x;
		global_reg->y = running->reg->y; 
		running->current_state = RUNNING;
		printf("Executando %s\n", running->name);

		int c;
		for (
			c = running->current_command;
			running->current_command <  c + global_quantum && running->current_command < running->command_count;
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
				block_process(BPQ, running, 3);
				break;
			}
			// Updates registers
			if (running->commands[running->current_command][0] == 'X')
			{
				global_reg->x = strtol(&running->commands[running->current_command][2], NULL, 10);
			}
			else if (running->commands[running->current_command][0] == 'Y')
			{
				global_reg->y = strtol(&running->commands[running->current_command][2], NULL, 10);
			}
			
			else if (strcmp(running->commands[running->current_command], "SAIDA")==0)
			{
				// The PCB's are never actually freed, instead they are marked as "FINISHED" which indicates
				// their slot in the process table is free to be assigned to a new incoming process.
				// This process is not implemented since this would be out of the scope of this assignment.
				running->current_state = FINISHED;
				printf("%s terminado. X=%d. Y=%d\n", running->name, global_reg->x, global_reg->y);
			}
			
		}
		
		if (running->current_state != FINISHED)
		{
			running->current_command - c == 1 ? 
			printf("Interrompendo %s apos 1 instrucao\n", running->name) :
			printf("Interrompendo %s apos %d instrucoes\n", running->name, running->current_command-c);
			if (running->current_state == RUNNING)
			{
				block_process(BPQ, running, 0);
			}
		}
	}
}

int main(int argc, char * argv[])
{

	// Open config files
	FILE * quantum_file = fopen("quantum.txt", "r");
    FILE * priorities_file = fopen("prioridades.txt", "r");
    
    // Set global variables
    total_interruptions = 0;
    global_reg = calloc(1, sizeof(registers));
    fscanf(quantum_file, "%d", &global_quantum);
    fclose(quantum_file);
    
	// Opening Log file
	char* log_file_name = malloc(12 + log10(global_quantum));
    log10(global_quantum) < 1 ?
    sprintf(log_file_name, "log0%d.txt", global_quantum) :
    sprintf(log_file_name, "log%d.txt", global_quantum);
    // Redirects stdout to "log_file" 
    FILE * log_file = freopen(log_file_name, "a", stdout);

    if (global_quantum > 999)
    {
    	// Let's not take any chances...
        fprintf(stderr, "Quantum too big!");
        exit(1);
    }
    
    FILE ** program_files = calloc(1000, sizeof(FILE*));
    program_count = get_program_files(program_files, argv[1]);
    process_table = malloc(sizeof(PCB)*program_count);

	// Loop through all "program_file_pointers" and initialize the programs in the Process Table
	for (int current_program = 0; current_program < program_count; current_program++)
    {
        process_table[current_program].name = malloc(64);
        fgets(process_table[current_program].name, 64, program_files[current_program]);
        process_table[current_program].name[strcspn(process_table[current_program].name, "\r\n")] = '\0';
		printf("Carregando %s\n", process_table[current_program].name);
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
		
		total_command_count += command_count;
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
        process_table[current_program].reg = calloc(1, sizeof(registers));
        
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

   	// Initializes a blocked process queue to be later filled
    blocked_process_queue * BPQ = initialize_blocked_process_queue();
    
    // Main Loop
    do
    {
  		// This loop consumes all ready processes and then re-insert them in the ready queue if possible
    	consume_ready_queue(r_queue, BPQ);
    	ready_process_buffer * newly_ready = malloc(sizeof(ready_process_buffer));
		newly_ready->ready_process_buffer = calloc(BPQ->size, sizeof(PCB*));
		newly_ready->size = 0;
		newly_ready->capacity = BPQ->size;
    	
    	for (blocked_process * BP = BPQ->HEAD; BP != NULL; BP = BP->next)
    	{
    		BP->blocked_process->current_state = READY;
    		BP->blocked_process->credit = BP->blocked_process->priority; 
			unblock_process(newly_ready, BPQ, BP);
    	}

    	for (int i=0; i<newly_ready->size; i++)
		{
			ready_queue_insert(r_queue, newly_ready->ready_process_buffer[i]);
		}
		
		free(newly_ready->ready_process_buffer);
    	free(newly_ready);
    } while (r_queue->size > 0);
    
    // Print statistics
    printf("MEDIA DE TROCAS: %.2f\n", (float) total_interruptions/program_count);
    printf("MEDIA DE INSTRUCOES: %.2f\n", (float) total_command_count/quantum_iter_count);
    printf("QUANTUM: %d\n", global_quantum);
    fclose(log_file);
    
    return 0;
}
