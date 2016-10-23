/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "window.h"
#include "config.h"

#include <SDL.h>

static SDL_Window *window;
static SDL_GLContext context;

static int screen_size[2];
static KeypressHandler keypress_handlers[16];
static unsigned int keypress_handlers_count = 0;
int first = 0;

void window_init(int fullscreen_ss)
{
	screen_size[0] = config_get_value_integer("res_width", 640);
	screen_size[1] = config_get_value_integer("res_height", 480);
	char fullscreen = config_get_value_integer("fullscreen", 0);

	SDL_Init(SDL_INIT_VIDEO);

#if defined SCREENSAVER
	if(fullscreen_ss)
	{
		fullscreen = 1;
		SDL_DisplayMode current;
		SDL_GetCurrentDisplayMode(0, &current);
		screen_size[0] = current.w;
		screen_size[1] = current.h;
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		fullscreen = 0;
		screen_size[0] = 800;
		screen_size[1] = 600;
	}
#endif

	Uint32 flags = SDL_WINDOW_OPENGL;
	if(fullscreen) flags |= SDL_WINDOW_FULLSCREEN;

	window = SDL_CreateWindow("GLMaze",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screen_size[0], screen_size[1], flags);
	context = SDL_GL_CreateContext(window);

	char vsync = config_get_value_integer("vsync", 1);
	SDL_GL_SetSwapInterval(vsync ? 1 : 0);
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
			SDL_Keycode key = ev.key.keysym.sym;
			if(key == SDLK_ESCAPE) return 0;

			unsigned int i;
			for(i=0; i<keypress_handlers_count; i++) keypress_handlers[i](key);
		}
#if defined SCREENSAVER
		if(ev.type == SDL_MOUSEBUTTONDOWN) return 0;
		if(ev.type == SDL_MOUSEMOTION)
		{
			if(ev.motion.xrel > 10 && ev.motion.yrel > 10 && first == 1)
			{
				return 0;
			}
			first = 1;
		}
#endif
	}
	return 1;
}

void window_swap_buffers()
{
	SDL_GL_SwapWindow(window);
}

void window_get_size(int size[2])
{
	size[0] = screen_size[0];
	size[1] = screen_size[1];
}
