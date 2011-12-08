#ifndef H_CONFIG
#define H_CONFIG

void config_load();
void config_print();
const char* config_get_value(char *name);
int config_get_value_integer(char *name, int def);
void config_set_value(char *name, char *value);
void config_set_value_integer(char *name, int value);

#endif //H_CONFIG
