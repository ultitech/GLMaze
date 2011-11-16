#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct Configuration
{
	char *name, *value;
};

struct Configuration *configurations;
int configurations_count;

static char* string_create_from(char *other, int count);
static int string_get_char_index(char *str, char c);

void config_load()
{
	char str[128];
	
	FILE *file = fopen("config.txt", "r");
	
	configurations_count = 0;
	while(fgets(str, 128, file) != NULL) configurations_count++;
	rewind(file);
	
	configurations = malloc(sizeof(struct Configuration) * configurations_count);
	int i = 0;
	while(fgets(str, 128, file) != NULL)
	{
		struct Configuration *config = &configurations[i++];
		int symbol_index = string_get_char_index(str, '=');
		config->name = string_create_from(str, symbol_index);
		config->value = string_create_from(str+symbol_index+1, strlen(str)-symbol_index-2);
	}
}

void config_print()
{
	int i;
	for(i=0; i<configurations_count; i++)
	{
		struct Configuration *config = &configurations[i];
		printf("%d: %s=%s\n", i, config->name, config->value);
	}
}

const char* config_get_value(char *name)
{
	int i;
	for(i=0; i<configurations_count; i++)
	{
		struct Configuration *config = &configurations[i];
		if(strcmp(config->name, name) == 0) return config->value;
	}
	return NULL;
}

int config_get_value_integer(char *name, int def)
{
	const char *value = config_get_value(name);
	if(value) return atoi(value);
	else return def;
}

static char* string_create_from(char *other, int count)
{
	char *str = malloc(count+1);
	int i;
	for(i=0; i<count; i++) str[i] = other[i];
	str[i] = '\0';
	return str;
}

static int string_get_char_index(char *str, char c)
{
	int i;
	for(i=0; str[i]!=c; i++);
	return i;
}
