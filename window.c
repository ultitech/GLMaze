#include "window.h"
#include "config.h"

#include <SDL/SDL.h>

int screen_size[2];
KeypressHandler keypress_handlers[16];
unsigned int keypress_handlers_count = 0;

void window_init()
{
	screen_size[0] = config_get_value_integer("res_width", 640);
	screen_size[1] = config_get_value_integer("res_height", 480);
	
#ifdef __APPLE__
	putenv("SDL_VIDEODRIVER=x11");
#endif
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, SDL_OPENGL);
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
			for(i=0; i<keypress_handlers_count; i++) keypress_handlers[i](key);
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
