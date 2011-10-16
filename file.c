#include "file.h"

#include <stdlib.h>
#include <stdio.h>

char* file_text(char *filename)
{
	FILE *file = fopen(filename, "r");
	if(!file) return NULL;
	
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	rewind(file);
	
	char *string = malloc(length+1);
	fread(string, 1, length, file);
	string[length] = '\0';
	
	fclose(file);
	
	return string;
}
