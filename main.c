#include "drawer.h"
#include "window.h"
#include "scene.h"

#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));
	
	file_set_resource_dir("./");
	file_set_output_dir("./");
	
	window_init();
	drawer_init();
	scene_init();
	
	char quit = 0;
	while(window_do_events())
	{	
		scene_update(0.02);
		
		drawer_begin_scene(0.02);
		scene_draw();
		drawer_end_scene();
		
		usleep(20000);
	}
	
	scene_quit();
	drawer_quit();
	window_quit();
	
	return 0;
}
