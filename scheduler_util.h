#include <stdio.h>

typedef struct PCB PCB;

FILE * log_file;
int get_program_files(FILE** program_file_pointers, char * path);
void print_statistics();
void end_process(PCB * p);
