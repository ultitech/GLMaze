/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ConfigEntry
{
	char *name, *value;
	struct ConfigEntry *next;
};

static struct ConfigEntry *config_first = NULL;
static struct ConfigEntry *config_last = NULL;

static void set_value_auto_free(char *name, char *value); //sets a config value, does not copy the strings, frees parameter-strings when they are not used
static struct ConfigEntry* get_config_entry_by_name(char *name);
static char* string_create_from(char *other, int count);
static int string_get_char_index(char *str, char c);

void config_load()
{
	FILE *file = fopen("config.txt", "r");
	char str[128];
	while(fgets(str, 128, file) != NULL)
	{
		if(strlen(str) == 1) continue; //contains only the newline character
		if(str[0] == '#') continue; //comment
		int symbol_index = string_get_char_index(str, '=');
		set_value_auto_free(string_create_from(str, symbol_index), string_create_from(str+symbol_index+1, strlen(str)-symbol_index-2));
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
	struct ConfigEntry *config = get_config_entry_by_name(name);
	if(config) return config->value;
	else return NULL;
}

int config_get_value_integer(char *name, int def)
{
	const char *value = config_get_value(name);
	if(value) return atoi(value);
	else return def;
}

void config_set_value(char *name, char *value)
{
	set_value_auto_free(strdup(name), strdup(value));
}

void config_set_value_integer(char *name, int value)
{
	char *value_s = malloc(16);
	sprintf(value_s, "%d", value);
	set_value_auto_free(strdup(name), value_s);
}

static void set_value_auto_free(char *name, char *value)
{
	struct ConfigEntry *config = get_config_entry_by_name(name);
	if(!config) //append new entry
	{
		config = malloc(sizeof(struct ConfigEntry));
		config->name = name;
		config->value = value;
		config->next = NULL;
		if(!config_first) config_first = config;
		if(config_last) config_last->next = config;
		config_last = config;
	}
	else
	{
		free(name); //we dont need the name
		free(config->value);
		config->value = value;
	}
}

static struct ConfigEntry* get_config_entry_by_name(char *name)
{
	struct ConfigEntry *config;
	for(config = config_first; config != NULL; config = config->next)
		if(strcmp(config->name, name) == 0) return config;
	return NULL;
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
