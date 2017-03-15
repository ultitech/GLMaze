# GLMaze

[![Travis-CI Status](https://travis-ci.org/ultitech/GLMaze.svg?branch=master)](https://travis-ci.org/ultitech/GLMaze)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/g9hi0c28c5ndv6a2?svg=true)](https://ci.appveyor.com/project/ultitech/glmaze)
[![Release](https://img.shields.io/badge/release-v1.0-blue.svg)](https://github.com/ultitech/GLMaze/releases/latest)
[![License: zlib](https://img.shields.io/badge/license-zlib-blue.svg)](http://www.zlib.net/zlib_license.html)

GLMaze is a cross-platform clone of the classic Windows 95 3D maze screensaver.

Be mesmerized as you walk through the seemingly endless maze, trying to find the exit. Pass a pyramid to turn your world upside down, but watch out for the puddles of water on the ground. You can even enter night vision to make you feel like a real spy.

![Screenshot with Nightvision](http://www.ultitech.de/media/images/glmaze_github1.jpg)
![Screenshot](http://www.ultitech.de/media/images/glmaze_github2.jpg)
![Screenshot with Nightvision and Motion Blur](http://www.ultitech.de/media/images/glmaze_github3.jpg)

## Getting Started

This project was created as a graphic demo to test the OpenGL 2.0 pipeline using SDL2 on Linux, Mac, and Windows. We included several shaders, such as motion blur, night vision as well as a 3D side-by-see mode.

The latest binary releases can be found [here](https://github.com/ultitech/GLMaze/releases/latest) or you can build the project yourself.

### Prerequisites

GLMaze requires [cmake](http://cmake.org) for compilation and depends on several third-party open source libraries:

+ [SDL 1.2](https://www.libsdl.org)
+ [GLEW](http://glew.sourceforge.net)
+ [FreeImage](http://freeimage.sourceforge.net)

#### Linux

+ build-essential

#### Mac

+ [Xcode](https://itunes.apple.com/us/app/xcode/id497799835?mt=12)
+ ([Sparkle](https://sparkle-project.org) - optional)

#### Windows

+ [Visual Studio](https://www.visualstudio.com/en-us/products/visual-studio-express-vs.aspx) **(recommended)** or
+ [MinGW](http://www.mingw.org)

### Compiling

#### Linux

If you haven't already, install the build-essentials and cmake, e.g.
```Bash
sudo apt-get install build-essential cmake
```

Install all required libraries using your preferred package manager, e.g.
```Bash
sudo apt-get install libsdl2-dev libfreeimage-dev libglew-dev
```

Next create your build directory inside the GLMaze folder and run `cmake ..`, e.g.
```Bash
mkdir build && cd build && cmake ..
```

Finally run `make` to compile the project.

#### Mac

If you haven't already, install Xcode and cmake.

Download the latest [SDL2 Development Library](https://www.libsdl.org/download-2.0.php) and copy the `SDL2.framework` framework into `/Library/Frameworks`.

Use [homebrew](http://brew.sh) to install the rest of the libraries, e.g.
```Bash
brew install freeimage glew
```

Next create your build directory inside the GLMaze folder and run `cmake ..`. This creates a UNIX Makefile; to create a Xcode project file instead, use `cmake -G "Xcode" ..`, e.g.
```Bash
mkdir build && cd build && cmake -G "Xcode" ..
```

Finally run `make` or open up the Xcode project, select the GLMaze target, and hit run.


#### Windows

##### Visual Studio
If you haven't already, install Visual Studio and cmake.

Download the latest libraries, place them in your prefered location, and set their path as environment variables (SDL2, FREEIMAGE, GLEW).

Next create your build directory inside the GLMaze folder and run `cmake ..`. To [specify](https://cmake.org/cmake/help/v3.0/manual/cmake-generators.7.html#ide-build-tool-generators) the Visual Studio version and build architecture use `cmake -G "Visual Studio ` followed by version, year, and architecture, e.g. `12 2013 Win64" ..`.

Finally open up the GLMaze solution and hit build.

### Configuration

You can change the resolution and turn on fullscreen using the [config.txt](res/config.txt) file. To load your own Shaders, place the files in the Shader subdirectory and add them to `pp_pipeline` separated by `;` like so `Filename;Hotkey`.

## Authors

Copyright © 2016 by [ultitech](https://www.ulti.tech)

* **Lukas Feller** - *Lead Programmer* - [luckyxxl](https://github.com/luckyxxl)
* **David Greiner** - *Programmer* - [USADavid](https://github.com/USADavid)

## License

This project is licensed under the zlib License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

Thanks to Microsoft for allowing us to spend countless hours being mesmerized by a screensaver when we were kids.
