/************************************
 EACH - USP | SISTEMAS DE INFORMAÇÃO
 EP1 - SO
 ANA POMARICO - 11208141
 BRUNO LEITE DE ANDRADE - 11369642
 DAYANE CARVALHO - 11319002
 VINÍCIUS M O DE MORAES - 11207748
 ************************************/

#include <stdio.h>

typedef struct PCB PCB;

FILE * log_file;
int get_program_files(FILE** program_file_pointers, char * path);
void print_statistics();
void end_process(PCB * p);

