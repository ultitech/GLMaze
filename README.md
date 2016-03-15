# GLMaze

[![Release](https://img.shields.io/badge/release-v0.1-blue.svg)](https://github.com/cxong/cdogs-sdl/releases/latest)
[![License: zlib](https://img.shields.io/badge/license-zlib-blue.svg)](http://www.zlib.net/zlib_license.html)

GLMaze is a cross-platform clone of the classic Windows 95 3D maze screensaver.

Be mesmerized as you walk through the seemingly endless maze, trying to find the exit. Pass a pyramid to turn your world upside down, but watch out for the puddles of water on the ground. You can even enter night vision to make you feel like a real spy.

![Screenshot with Nightvision](http://www.ultitech.de/media/images/glmaze_github1.jpg)
![Screenshot](http://www.ultitech.de/media/images/glmaze_github2.jpg)
![Screenshot with Nightvision and Motion Blur](http://www.ultitech.de/media/images/glmaze_github3.jpg)

## Getting Started

This project was created as a graphic demo to test the OpenGL 2.0 pipeline using SDL on Linux, Mac, and Windows. We included several shaders, such as motion blur, night vision as well as a 3D side-by-see mode.

The latest binary releases can be found [here](http://link) or you can build the project yourself.

### Prerequisites

GLMaze depends on several open source libraries

+ [SDL 1.2](https://www.libsdl.org)
+ [GLEW](http://glew.sourceforge.net)
+ [FreeImage](http://freeimage.sourceforge.net)

#### Mac

+ ([Sparkle](https://sparkle-project.org) - optional)

#### Windows

+ [MinGW](http://www.mingw.org)

### Installing

#### Linux

Install all required libraries using your preferred package manager. Then run `make`.

#### Mac

Install all required libraries using brew or compile them on your own. Open the Xcode project and run.

#### Windows

First install MinGW, then install all required libraries. Finally run `make -f makefile.win` to compile.

### Configuration

You can change the resolution and turn on fullscreen using the [context.txt](context.txt) file. To load your own Shaders, place the files in the Shader subdirectory and add them to `pp_pipeline` separated by `;` like so `Filename;Hotkey`.

## Authors

Copyright © 2016 by [ultitech](https://www.ulti.tech)

* **Lukas Feller** - *Lead Developer* - [luckyxxl](https://github.com/luckyxxl)
* **David Greiner** - *Mac port* - [USADavid](https://github.com/USADavid)

## License

This project is licensed under the zlib License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

Thanks to Microsoft for allowing us to spend countless hours being mesmerized by a screensaver when we were kids.