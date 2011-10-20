#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *prefix_texture = "Textures/";
char *prefix_shader = "Shader/";

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

char* file_prefix(char *filename, enum FilePrefix prefix)
{
	char *prefix_str;
	static char *str;
	
	if(str) free(str);
	
	switch(prefix)
	{
		case PREFIX_TEXTURE:
		prefix_str = prefix_texture;
		break;
		
		case PREFIX_SHADER:
		prefix_str = prefix_shader;
		break;
	}
	
	str = malloc(strlen(prefix_str)+strlen(filename)+1);
	strcpy(str, prefix_str);
	strcat(str, filename);
	
	return str;
}
