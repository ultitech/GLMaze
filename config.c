#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ConfigEntry
{
	char *name, *value;
	struct ConfigEntry *next;
};

struct ConfigEntry *config_first = NULL;

static char* string_create_from(char *other, int count);
static int string_get_char_index(char *str, char c);

void config_load()
{
	FILE *file = fopen("config.txt", "r");
	struct ConfigEntry *last = NULL;
	char str[128];
	while(fgets(str, 128, file) != NULL)
	{
		struct ConfigEntry *new_entry = malloc(sizeof(struct ConfigEntry));
		int symbol_index = string_get_char_index(str, '=');
		new_entry->name = string_create_from(str, symbol_index);
		new_entry->value = string_create_from(str+symbol_index+1, strlen(str)-symbol_index-2);
		new_entry->next = NULL;
		if(!config_first) config_first = new_entry;
		if(last) last->next = new_entry;
		last = new_entry;
	}
}

void config_print()
{
	struct ConfigEntry *config;
	for(config = config_first; config != NULL; config = config->next)
		printf("%s=%s\n", config->name, config->value);
}

const char* config_get_value(char *name)
{
	struct ConfigEntry *config;
	for(config = config_first; config != NULL; config = config->next)
		if(strcmp(config->name, name) == 0) return config->value;
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
