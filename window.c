/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "window.h"
#include "config.h"

#include <SDL/SDL.h>

static int screen_size[2];
static KeypressHandler keypress_handlers[16];
static unsigned int keypress_handlers_count = 0;

void window_init()
{
	screen_size[0] = config_get_value_integer("res_width", 640);
	screen_size[1] = config_get_value_integer("res_height", 480);
	char fullscreen = config_get_value_integer("fullscreen", 0);
	
	SDL_Init(SDL_INIT_VIDEO);
	Uint32 flags = SDL_OPENGL;
	if(fullscreen) flags |= SDL_FULLSCREEN;
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, flags);
	SDL_WM_SetCaption("GLMaze", NULL);
}

void window_quit()
{
	SDL_Quit();
}

void window_add_keypress_handler(KeypressHandler handler)
{
	keypress_handlers[keypress_handlers_count++] = handler;
}

int window_do_events()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT) return 0;
		if(ev.type == SDL_KEYDOWN)
		{
			SDLKey key = ev.key.keysym.sym;
			if(key == SDLK_ESCAPE) return 0;
			
			int i;
			for(i=0; i<keypress_handlers_count; i++) keypress_handlers[i]((enum Key)key);
		}
	}
	return 1;
}

void window_swap_buffers()
{
	SDL_GL_SwapBuffers();
}

void window_get_size(int size[2])
{
	size[0] = screen_size[0];
	size[1] = screen_size[1];
}
