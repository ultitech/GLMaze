#include "drawer.h"
#include "window.h"
#include "scene.h"
#include "config.h"
#include "file.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
