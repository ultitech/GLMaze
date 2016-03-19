/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "drawer.h"
#include "window.h"
#include "scene.h"
#include "config.h"
#include "file.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL2/SDL.h> //for SDL_GetTicks
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

int main(int argc, char *argv[])
{
	srand(time(NULL));
	
#if defined __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        printf("cannot change working directory, check permissions\n");
        exit(0);
    }
    CFRelease(resourcesURL);
    
    chdir(path);
#endif
    
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
