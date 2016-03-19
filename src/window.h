/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef H_WINDOW
#define H_WINDOW

#include <SDL2/SDL_keycode.h>

typedef void(*KeypressHandler)(SDL_Keycode);

void window_init();
void window_quit();
void window_add_keypress_handler(KeypressHandler handler);
int window_do_events();
void window_swap_buffers();
void window_get_size(int size[2]);

#endif
