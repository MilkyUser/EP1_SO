#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "scheduler.h"

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
	
  	qsort(program_names, program_count, sizeof(const char**), compare_program_files_names);
	
  	for (int i=0; i<program_count; i++)
  	{
		char * program_path = calloc(256, 1);
		sprintf(program_path, "%s%s", path, program_names[i]);
	    program_file_pointers[i] = fopen(program_path, "r");
	    free(program_path);
	    free(program_names[i]);
  	}
  	
  	free(dir);
  	free(program_names);
  	return program_count;
}
#endif

int main(int argc, char* argv[])
{
    FILE* quantum_file = fopen("quantum.txt", "r");
    FILE* priorities_file = fopen("prioridades.txt", "r");
    int quantum;
    fscanf(quantum_file, "%d", &quantum);
    fclose(quantum_file);
    
    if (quantum > 999)
    {
        fprintf(stderr, "Quantum too big!");
        exit(1);
    }
	
    FILE ** program_files = calloc(1000, sizeof(FILE*));
    int program_count = get_program_files(program_files, argv[1]);
    program * programs = malloc(sizeof(program)*program_count);
	
    for (int current_program = 0; current_program < program_count; current_program++)
    {
        programs[current_program].name = malloc(64);
        fgets(programs[current_program].name, 64, program_files[current_program]);
        programs[current_program].name[strcspn(programs[current_program].name, "\r\n")] = '\0';

        if (!feof(priorities_file))
        {
            unsigned char * value = malloc(1);
            fscanf(priorities_file, "%d", value);
            if (*value > 255 || *value < 0)
            {	
                fprintf(stderr, "PRIORIDADE %d INVALIDA NA LINHA %d DO ARQUIVO DE PRIORIDADES\n", value, current_program);
                exit(1);
            }
            programs[current_program].priority = *value;
        }
        else
        {
            printf(stderr, "ERRO: ARQUIVO DE PRIORIDADES CONTEM MENOS PRIORIDADES QUE A"
                            " QUANTIDADE DE PROGRAMAS NA PASTA:\n %s\n", argv[1]);
            exit(1);
        }

        int command_count = 0;
        while (!feof(program_files[current_program]))
        {
            if (getc(program_files[current_program]) == '\n')
            {
                command_count++;
            }
        }
		
        programs[current_program].command_count = command_count;
        fseek(program_files[current_program], strlen(programs[current_program].name), SEEK_SET);
        programs[current_program].commands = malloc(16*command_count);
        for (int current_command=0; current_command<command_count;current_command++)
        {
            char * command = malloc(16);
            fscanf(program_files[current_program], "%s", command);
            programs[current_program].commands[current_command] = command;
        }
        fclose(program_files[current_program]);
    }
    free(program_files);
    
    program_queue * p_queue = calloc(1, sizeof(program_queue));
    p_queue->size = 0;
	p_queue->root = calloc(1000, sizeof(program));
    for (int i=0; i<program_count; i++)
    {	
    	insert(p_queue, &programs[i]);
    }    
	
    // TODO: TUDO KKKK
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
