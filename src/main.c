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

#if defined SCREENSAVER
enum screensaverParameter { NONE, CONFIGURATION, PREVIEW, FULLSCREEN };

static enum screensaverParameter get_screensaver_parameter(int argc, const char *argv[])
{
	if(argc == 1) return CONFIGURATION;
	else if(argc == 2)
	{
		char* arg = argv[1];
		if(arg[0] == '-' || arg[0] == '/')
		{
			switch(tolower(arg[1]))
			{
				case 'c':
					return CONFIGURATION;
				case 'p':
					return PREVIEW; // Preview in Settings screen
				case 's':
					return FULLSCREEN; // When user clicks Preview button in Settings screen
			}
		}
	}

	return NONE;
}
#endif

static void run()
{
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
}

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

	//TODO: instead of chdir, pass the path here
	file_set_resource_dir("./");

	file_set_output_dir("./");

#if defined SCREENSAVER
	enum screensaverParameter parameter = get_screensaver_parameter(argc, argv);

	switch(parameter)
	{
		case NONE:
			break;
		case CONFIGURATION:
			system("notepad config.txt");
			break;
		case PREVIEW:
			//SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "No preview", "This screensaver can not be previewed.", NULL);
			break;
		case FULLSCREEN:
			run();
			break;
	}
#else
	run();
#endif

	return 0;
}
