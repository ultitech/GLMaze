/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static char *resource_dir, *output_dir;
static char *prefix_texture = "Textures/";
static char *prefix_shader = "Shader/";

char* file_text(char *filename)
{
	FILE *file;
	errno_t err;
	if ((err = fopen_s(&file, filename, "rb")) != 0) {
		printf("%s could not be opened", filename);
		return NULL;
	}
	
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	rewind(file);
	
	char *string = malloc(length+1);
	int result = fread(string, 1, length, file);
	if(result != length) return NULL;
	
	string[length] = '\0';
	
	fclose(file);
	
	return string;
}

void file_set_resource_dir(char *dir)
{
	resource_dir = malloc(strlen(dir)+1);
	strcpy(resource_dir, dir);
}

char* file_resource(char *filename, enum FileResource type)
{
	static char *str;
	
	if(str) free(str);
	
	char *prefix_str = NULL;
	switch(type)
	{
		case RESOURCE_TEXTURE:
		prefix_str = prefix_texture;
		break;
		
		case RESOURCE_SHADER:
		prefix_str = prefix_shader;
		break;
	}
	
	str = malloc(strlen(resource_dir)+strlen(prefix_str)+strlen(filename)+1);
	strcpy(str, resource_dir);
	strcat(str, prefix_str);
	strcat(str, filename);
	
	return str;
}

void file_set_output_dir(char *dir)
{
	output_dir = malloc(strlen(dir)+1);
	strcpy(output_dir, dir);
}

char* file_output(char *filename)
{
	static char *str;
	
	if(str) free(str);
	
	str = malloc(strlen(output_dir)+strlen(filename)+1);
	strcpy(str, output_dir);
	strcat(str, filename);
	
	return str;
}
