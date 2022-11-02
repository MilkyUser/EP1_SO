#include <stdio.h>
#include "scheduler_util.h"

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
