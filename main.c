#include "drawer.h"
#include "window.h"
#include "scene.h"
#include "config.h"
#include "file.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL/SDL.h> //for SDL_GetTicks

int main()
{
	srand(time(NULL));
	
	file_set_resource_dir("./");
	file_set_output_dir("./");
	
	config_load();
	config_print();
	
	window_init();
	drawer_init();
	scene_init();
	
	float time_passed = 0.0;
	
	while(window_do_events())
	{
		int start = SDL_GetTicks();
		
		scene_update(time_passed);
		
		drawer_begin_scene(time_passed);
		scene_draw();
		drawer_end_scene();
		
		usleep(20000);
		
		int end = SDL_GetTicks();
		time_passed = (end-start)/1000.0;
	}
	
	scene_quit();
	drawer_quit();
	window_quit();
	
	return 0;
}
