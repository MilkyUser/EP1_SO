#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "scheduler.h"
#include "ready_queue.h"
#include "blocked_queue.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <fileapi.h>
#define sch_sleep Sleep
#define tick_speed 3000
#define path_sep_ascii 92

int get_program_files(FILE** program_file_pointers, char * path)
{
    char * path_handler = malloc(1000);
    sprintf(path_handler, "%s\\*", path);
    int program_count = 0;

    // This snippet was extracted, and subsequently modified, from:
    // https://stackoverflow.com/questions/41404711/how-to-list-files-in-a-directory-using-the-windows-api
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(path_handler, &data);
    FindNextFile(hFind, &data);
    if ( hFind != INVALID_HANDLE_VALUE) {
        while (FindNextFile(hFind, &data)) {

            char * path_and_name = malloc(1000);
            sprintf(path_and_name, "%s\\%s", path, data.cFileName);
            program_file_pointers[program_count] = fopen(path_and_name, "r");
            program_count++;
            free(path_and_name);
        }
        FindClose(hFind);
    }
    free(path_handler);
    return program_count;
}
#else
#include <dirent.h>
#define _DEFAULT_SOURCE
#define sch_sleep uSleep
#define tick_speed 3
#define path_sep_ascii 47

// Utility  function to compare strings
// This function wraps the strcmp function and compare the given strings in uppercase
int compare_program_files_names(const char ** str1, const char ** str2)
{	
	char cstr1[256];
	char cstr2[256];
	
	for (int i=0; i<strlen(*str1); i++)
	{	
		cstr1[i] = toupper((*str1)[i]);
	}
	cstr1[strlen(*str1)] = '\0';
		
	for (int i=0; i<strlen(*str2); i++)
	{
		cstr2[i] = toupper((*str2)[i]);
	}
	cstr2[strlen(*str2)] = '\0';
	
	return strcmp(cstr1, cstr2);
}

int get_program_files(FILE** program_file_pointers, char * path)
{
	// This snippet was extracted, and subsequently modified, from:
    // https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
	short program_count = 0; 
	DIR *d;
	struct dirent *dir;
 	d = opendir(path);
 	const char ** program_names = calloc(1000, sizeof(const char *));
	if (d)
	{
	    while ((dir = readdir(d)) != NULL)
	    {	
	    	if (dir->d_name[0] == '.')
	    	{
	    		continue;
	    	}
	    	program_names[program_count] = calloc(256, 1);
	    	sprintf(program_names[program_count], "%s\0", dir->d_name);
			program_count++;
    	} 
    closedir(d);
  	}
	
	// The files are sorted in alphabetical order
	// It took me a week to figure out this function .-.
  	qsort(program_names, program_count, sizeof(const char**), compare_program_files_names);
	
  	for (int i=0; i<program_count; i++)
  	{
		char * program_path = calloc(256, 1);
		sprintf(program_path, "%s/%s", path, program_names[i]);
	    program_file_pointers[i] = fopen(program_path, "r");
	    free(program_path);
	    free(program_names[i]);
  	}
  	
  	free(dir);
  	free(program_names);
  	return program_count;
}
#endif

registers global_reg = {NULL, NULL};

int main(int argc, char* argv[])
{

	// Open config files
	FILE * quantum_file = fopen("quantum.txt", "r");
    FILE * priorities_file = fopen("prioridades.txt", "r");
    int quantum;
    fscanf(quantum_file, "%d", &quantum);
    fclose(quantum_file);
    
    if (quantum > 999)
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
            process_table[current_program].quantum = quantum;
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
   	
    test_drive_2();

	FILE* logf;
    char* log_file_name = malloc(12 + log10(quantum));
    log10(quantum) < 1 ?
    sprintf(log_file_name, "log0%d.txt", quantum) :
    sprintf(log_file_name, "log%d.txt", quantum);
    logf = fopen(log_file_name, "a");
    fprintf(logf, "QUANTUM: %d\n", quantum);
    fclose(logf);
    
    return 0;
}
