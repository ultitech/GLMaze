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
#ifdef _WIN32
#include <string.h>
#include <windows.h>
#endif

#include <SDL.h> //for SDL_GetTicks
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

int main(int argc, char *argv[])
{
	srand(time(NULL));

#if defined __APPLE__
	{
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
	}
#endif
#if defined _WIN32
	{
		TCHAR dest[ MAX_PATH ];
		GetModuleFileName( NULL, dest, MAX_PATH );
		char drive[ _MAX_DRIVE ];
		char dir[ _MAX_DIR ];
		char fname[ _MAX_FNAME ];
		char ext[ _MAX_EXT ];
		_splitpath_s( dest, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT );
		chdir( dir );
	}
#endif

#if defined SCREENSAVER
	enum screensaverParameter { NONE, CONFIGURATION, PREVIEW, FULLSCREEN };
	enum screensaverParameter parameter = NONE;
	if(argc > 1)
	{
		char* cmdLine = argv[1];
		if(cmdLine[0] == '-' || cmdLine[0] == '/')
		{
			switch(tolower(cmdLine[1]))
			{
			case 'c':
				parameter = CONFIGURATION;
				break;
			case 'p':
				parameter = PREVIEW; // Preview in Settings screen
				break;
			case 's':
				parameter = FULLSCREEN; // When user clicks Preview button in Settings screen
				break;
			default:
				parameter = NONE;
			}
		}
	}

	if(argc == 1 || parameter == CONFIGURATION)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "No configuration", "This screensaver can not be configured.", NULL);
	}
	else if(parameter == PREVIEW)
	{
		//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "No preview", "This screensaver can not be previewed.", NULL);
	}
	else if(parameter == FULLSCREEN)
	{
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

		int end = SDL_GetTicks();
		time_passed = (end-start)/1000.0;
	}

	scene_quit();
	drawer_quit();
	window_quit();

#if defined SCREENSAVER
	}
#endif

	return 0;
}
