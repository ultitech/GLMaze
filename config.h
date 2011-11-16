#ifndef H_CONFIG
#define H_CONFIG

void config_load();
void config_print();
const char* config_get_value(char *name);
int config_get_value_integer(char *name, int def);

#endif //H_CONFIG
