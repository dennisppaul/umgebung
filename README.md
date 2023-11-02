*Umgebung* is a light-weight C++ environment for small audio and graphics based applications. it is somewhat inspired by [Processing.org](https://processing.org) and the like.

## Prerequisite ( for MacOS )

this project relies on the following packages:

- [CMake](https://cmake.org/)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [GLFW](https://www.glfw.org/) 
- [PortAudio](https://www.portaudio.com) 
- [FTGL](https://github.com/frankheckenbach/ftgl) 
- [GLEW](https://glew.sourceforge.net/)

in order to compile and run applications install the following packages with [Homebrew](https://brew.sh):

```
$ brew install cmake pkgconfig glfw portaudio ftgl glew
```

## Build Applications with *Umgebung*

an example application can be found in the `example` folder. to run the example application do the following:

```
$ cd example
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./umgebung-example-app
```

if changes are made to `umgebung-example-app.cpp‌` ( or any other file in that folder ) it is enough to just run:

```
$ make ; ./umgebung-example-app
```

## Known Issues

- only tested on MacOS. although theoretically the external libraries as well as the build system should be cross-platform ( i.e Macos, Linux, Windows )
- portaudio is manually configured because it does not work on all machines. this *dirty hack* is somewhat buggy and sometimes requires to call `cmake ..` again after `make` fails once
- on some clean homebrew installation `‌LIBRARY_PATH` is not set. if so make sure it is set or add the line `export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib` to your profile ( e.g in to `~/.zshrc` )