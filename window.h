#ifndef H_WINDOW
#define H_WINDOW

#include "key.h"

typedef void(*KeypressHandler)(enum Key);

void window_init();
void window_quit();
void window_add_keypress_handler(KeypressHandler handler);
int window_do_events();
void window_swap_buffers();
void window_get_size(int size[2]);

#endif
