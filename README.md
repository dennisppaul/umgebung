*Umgebung* is a light-weight C++ environment for small audio and graphics based applications. it is somewhat inspired by [Processing.org](https://processing.org) and the like. 

well, *Umgebung* is actually imitating the [Processing.org](https://processing.org) quite bluntly ( e.g the main application class is called `PApplet` ).

this project tries to supply a framework that allow to write applications that more or less look like original [Processing.org](https://processing.org) sketches. note the character of this project is that features will be added as they are needed. it might partlu replicate but not fully emulate the original [Processing.org](https://processing.org) environment.

*Umgebung* may also function as a statement on what is good and valueable about the [Processing.org](https://processing.org) *idiom* while at the same time suggesting to disconnecting it from one specific group, application or project … maybe a bit like markdown in that sense.

see [DOCUMENTATION](./DOCUMENTATION.md) for usage information.

## Cloning with submodules

in order to clone the project with submodules ( e.g in example `umgebung-with-klangwellen` which requires the [klangwellen](https://github.com/dennisppaul/klangwellen) library ) use the following command:

```
$ git clone --recurse-submodules https://github.com/dennisppaul/umgebung.git
```

## Prerequisite

this project relies on the following packages:

- [CMake](https://cmake.org/)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)
- [SDL2](https://www.libsdl.org)
- [FTGL](https://github.com/frankheckenbach/ftgl)
- [GLEW](https://glew.sourceforge.net/)
- [oscpack](http://www.rossbencina.com/code/oscpack) ( included as source files )
- [ffmpeg](https://ffmpeg.org)
- [rtmidi](https://github.com/thestk/rtmidi)

### macOS

in order to compile and run applications install the following packages with [Homebrew](https://brew.sh):

```
$ brew install cmake pkgconfig sdl2 ftgl glew ffmpeg rtmidi
```

( or run `brew bundle` in project directory. )

### on Linux

on linux ( including Raspberry Pi OS ) install the required packages with [APT](https://en.wikipedia.org/wiki/APT_(software)):

```
$ sudo apt-get update -y
$ sudo apt-get upgrade -y
$ sudo apt-get install cmake pkg-config libsdl2-dev libglfw3-dev libftgl-dev libglew-dev ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev librtmidi-dev -y
$ sudo apt-get install git clang mesa-utils # optional
```

### on Windows

- install [MSYS2](https://www.msys2.org/)
- install `MinGW` from `MSYS2` terminal with `‌pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain`
- e.g use this instruction https://code.visualstudio.com/docs/cpp/config-mingw

## Build Applications with *Umgebung*

an example application can be found in the `example` folder. to run the example application do the following:

```
$ cd ./examples/umgebung-app/
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./umgebung-app
```

if changes are made to `umgebung-app.cpp` ( or any other file in that folder ) it is enough to just run:

```
$ make ; ./umgebung-app
```

## Known Issues

- a LOT of functions + methods + strategies are not yet implemented ( the idea is to implement these on demand )
- color system is currently fixed to rang `0 ... 1` and only works with RGB(A)
- elements in `println()` need to be concatenated with `,` and not `+` e.g `println("hello ", 23, " world");`
- only tested on macOS + Raspberry Pi OS. although theoretically the external libraries as well as the build system should be cross-platform ( i.e macOS, Windows and any UNIX-like system )

### Setting up Homebrew on macOS

on some *clean* homebrew installations on macOS the environment variable `$LIBRARY_PATH` is not set or at least does not include the
homebrew libraries. if so you may add the line `export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib` to your profile e.g in `~/.zshrc` in *zsh* shell. note, that other shell environments use other profile files and mechanisms e.g *bash* uses `~/.bashrc`. find out which shell you are using by typing `echo $0`.

if you have NO idea what this all means you might just try the following lines ( as always without the `$` ) for *zsh*:

```
$ { echo -e "\n# set library path\n"; [ -n "$LIBRARY_PATH" ] && echo "export LIBRARY_PATH=/usr/local/lib:\"\$LIBRARY_PATH\"" || echo "export LIBRARY_PATH=/usr/local/lib"; } >> "$HOME/.zshrc"
$ source "$HOME/.zshrc"
```

this will set the `$LIBRARY_PATH` in your profile file.
