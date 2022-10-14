#include <stdio.h>
#include <math.h>
#include "scheduler.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <fileapi.h>
#define sch_sleep Sleep
#define tick_speed 3000
#define path_sep_ascii 92

int get_program_files(FILE** program_file_pointers, char * path){
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
            fopen_s(&program_file_pointers[program_count], path_and_name, "r");
            program_count++;
        }
        FindClose(hFind);
    }
    return program_count;
}
#else
#include <unistd.h>
#define sch_sleep uSleep
#define tick_speed 3
#define path_sep_ascii 47
FILE ** get_program_files(){
    return void;
}
#endif


int main(int argc, char* argv[])
{
    FILE* current_file;
    fopen_s(&current_file, "quantum.txt", "r");
    int quantum;
    fscanf_s(current_file, "%d", &quantum);
    fclose(current_file);

    if (quantum > 999)
    {
        fprintf(stderr, "Quantum too big!");
        exit(1);
    }

    fopen_s(&current_file, "prioridades.txt", "r");
    FILE ** program_files = calloc(1000, sizeof(FILE**));
    int program_count = get_program_files(program_files, argv[1]);
    program * programs = malloc(sizeof(program)*program_count);

    for (int current_program = 0; current_program < program_count; current_program++)
    {
        programs[current_program].name = malloc(64);
        fscanf(program_files[current_program], "%s", programs[current_program].name);

        if (!feof(current_file))
        {
            long * value = malloc(sizeof(int));
            fscanf(current_file, "%d", value);

            if (*value > 255 || *value < 0)
            {
                printf(stderr, "PRIORIDADE INVALIDA NA LINHA %d DO ARQUIVO DE PRIORIDADES\n");
                exit(1);
            }
            programs[current_program].priority = *value;
        }
        else
        {
            fprintf(stderr, "ERRO: ARQUIVO DE PRIORIDADES CONTEM MENOS PRIORIDADES QUE A"
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
        command_count--;
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
        free(program_files[current_program]);
    }
    free(program_files);

    // TODO: TUDO KKKK

    FILE* log;
    char* log_file_name = malloc(12 + (int) log10(quantum));
    log10(quantum) < 1 ?
    sprintf(log_file_name, "log0%d.txt", quantum) :
    sprintf(log_file_name, "log%d.txt", quantum);
    fopen_s(&log, log_file_name, "a");
    fprintf(log, "QUANTUM: %d\n", quantum);
    fclose(log);
    return 0;
}
