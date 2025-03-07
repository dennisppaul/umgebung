*Umgebung* is a lightweight C++ environment for small audio and graphics based applications. it is inspired
by [Processing.org](https://processing.org) and similar environments.

well, *Umgebung* is actually imitating [Processing.org](https://processing.org) quite bluntly ;)

this project aims to supply a framework that allows writing applications that more or less look like
original [Processing.org](https://processing.org) sketches. note the character of this project is that features will be
added as they are needed. it might partly replicate but not fully emulate the
original [Processing.org](https://processing.org) environment.

*Umgebung* may also function as a statement on what is good and valuable about
the [Processing.org](https://processing.org) *idiom* while at the same time suggesting disconnecting it from one
specific group, application or project … maybe a bit like markdown in that sense.

see [DOCUMENTATION](./DOCUMENTATION.md) for usage information
and [examples](https://github.com/dennisppaul/umgebung-examples) for applications.

## Why *Umgebung*?

*Umgebung* is yet another programming environment for designers, makers, and artists. so why does it exist? and why not
use [Processing.org](https://processing.org), [OpenFrameworks](https://openframeworks.cc), [Cinder](https://libcinder.org)
or any of the other frameworks out there?

well, the main reason why we even created *Umgebung* was basically to have a C++ derivative of Processing.org that stays
very close to the *original* idiom. while Cinder and OpenFrameworks, for example, are fantastic projects, they differ
quite a lot from the *Processing.org Idiom* ( i.e a `setup()-draw()` structure, `stroke()+fill()` and shape-based
drawing, etcetera ). *Umgebung*, however, literally imitates core Processing classes and functions.

why not use the original Java-based *Processing.org* then? there are a few key aspects that just cannot and probably
will never be resolved due to limitations imposed by Java. while Java is a very well-designed language and does many
things better than C/C++, it has some drawbacks.

the most important benefit of moving to C/C++ is the availability of many extremely powerful and widespread libraries
and APIs ( e.g OpenGL, OpenCV, FFmpeg, PortAudio ) that can be natively used and integrated into applications and
sketches without the need for a native binding library. while many of these libraries have been fully or partly made
accessible in Processing through native bindings, oftentimes these libraries or their bindings introduce significant
overhead ( technically, administratively, etcetera ), are not at the current version, are incompatible with newer
versions of OSes, or do not expose all functionality ( e.g unstable webcam support on macOS ). while this also happens
with C/C++ libraries and APIs, the problems are greatly reduced. also, there is still a significant number of libraries
and APIs not available for Java-based Processing or that are impossible to port because of limitations posed by Java VM
implementations ( e.g multi-channel audio ).

in addition to this, due to the nature of C/C++ and the way *Umgebung* is implemented, applications and sketches
developed with *Umgebung* can be built and deployed with a very(!) small memory and CPU footprint. this means that
applications and sketches can potentially run on smaller or older hardware like, e.g Raspberry Pi. some modules can even
be excluded from a build to reduce the footprint even more. *Umgebung* can even run *truly* headless ( i.e it does not
require any virtual offscreen graphics driver if no window is required ).

furthermore, *Umgebung* uses CMake as a build system. CMake is very well-designed and more or less easy to learn and
extend. this also means that *Umgebung* applications and sketches can be developed in more or less any text editor or
IDE ( e.g Visual Studio Code, Zed, CLion, Vim ). it does not require proprietary build systems like Xcode.

and finally, although C and especially C++ is known for its horrific errors, confusing error messages, and volatile
behavior at times, with the advent of LLM-based programming assistants, the entry threshold can be greatly reduced.
trust the process.

## Prerequisite

this project relies on the following packages:

- [CMake](https://cmake.org/) `@version(3.30.3)`
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) `@version(0.29.2)`
- [SDL2](https://www.libsdl.org) `@version(2.30.7)`
- [FTGL](https://github.com/frankheckenbach/ftgl) `@version(2.1.3)`
- [GLEW](https://glew.sourceforge.net/) `@version(2.2.0)`
- [ffmpeg](https://ffmpeg.org) `@version(7.0.2)`
- [rtmidi](https://github.com/thestk/rtmidi) `@version(6.0.0)`
- [oscpack](http://www.rossbencina.com/code/oscpack) ( included as source files )
- [dr_libs](https://github.com/mackron/dr_libs) ( included as source files )
- [dylibbundler](https://github.com/auriamg/macdylibbundler) ( only required for bundling standalone macOS applications.
  it can also be installed via homebrew with `brew install dylibbundler` ) `@version(1.0.5)`
- [PortAudio](https://www.portaudio.com) ( can be used as a replacement for SDL2-based audio. in contrast to SDL2 it can
  output audio to multiple channels. it can also be installed via homebrew with `brew install portaudio` )
  `@version(19.7.0)`

( `@version(X.X.X)` states the version number of the library with which *Umgebung* has been tested on macOS. other
versions might work as well but are not guaranteed to. )

### macOS

in order to compile and run applications install the following packages with [Homebrew](https://brew.sh):

either manually with:

```
$ brew install cmake pkgconfig sdl2 ftgl glew ffmpeg rtmidi
```

or run installer script `./install-macOS.sh` ( i.e checking for Homebrew and running the bundler with `brew bundle` ).

### Linux

on linux ( including Raspberry Pi OS ) install the required packages
with [APT](https://en.wikipedia.org/wiki/APT_(software)):

```
$ sudo apt-get update -y
$ sudo apt-get upgrade -y
$ sudo apt-get install git clang mesa-utils # optional -y
$ sudo apt-get install cmake pkg-config libsdl2-dev libftgl-dev libglew-dev ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev librtmidi-dev -y
```

alternatively, run installer script `./install-linux.sh` to install packages
with [Linuxbrew](https://docs.brew.sh/Homebrew-on-Linux) ( linux version of Homebrew, currently not supported on
Raspberry Pi OS ).

#### Raspberry Pi OS (RPI)

RPI currently uses X11 as the rendering system.

also, RPI does not support antialiasing i.e make sure to set the value to `0` in `settings`:

```cpp
void settings(){
    antialiasing = 0;
}
```

*Umgebung* was tested on Raspberry Pi 4 ( Model B ) with Raspberry Pi OS (64-bit) "Bookworm" ( Release 2024-07-04 ).
however, it has not been tested carefully. there might be glitches …

### Windows

- install [MSYS2](https://www.msys2.org/)
- install the following modules with `pacman` in `MSYS2 UCRT64`:

```
$ pacman -Syu --noconfirm
$ pacman -S --noconfirm mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake git
$ pacman -S --noconfirm mingw-w64-ucrt-x86_64-glew mingw-w64-ucrt-x86_64-mesa mingw-w64-ucrt-x86_64-ftgl mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-ffmpeg mingw-w64-ucrt-x86_64-rtmidi
```

the setup is exclusively for the `MSYS2 UCRT64` branch ( and not for `MSYS2 MINGW64` etcetera ). also it uses `ninja` as
a build system instead of `make` ( which is the default on linux + macOS ).

## Building Applications with *Umgebung*

example applications can be found in the dedicated
repository [umgebung-examples](https://github.com/dennisppaul/umgebung-examples). first, make sure that both repositories are cloned into the same location, next to each other ( the examples assume umgebung library to be next to the examples folder. this can be changed in the `CMakeLists.txt` of each example )

```
.
├── umgebung
└── umgebung-examples
```

to run example `umgebung-simple` do the
following:

```
$ cd ./umgebung-examples/examples/umgebung-simple/
$ cmake -B build .         # prepare build system
$ cmake --build build      # build application
$ ./build/umgebung-simple  # run application 
```

if changes are made to `umgebung-simple.cpp` ( or any other file in that folder ) it is enough to just run:

```
$ make -C build ; ./build/umgebung-simple
```

## Known Differences

Java and C++ are similar in some aspects, but are very different in many others. a lot of the syntactic differences
between *Processing* and *Umgebung* stem from these differences, while a few others are more or less developer
decisions.

- elements in `println()` must be concatenated with `,` rather than `+` e.g `println("hello ", 23, " world");` @reason(
  strings cannot be concatenated with a `+` in C++ as they can be in Java)
- the equivalent property to `mousePressed()` is renamed to `isMousePressed` @reason(in *Processing* the property
  `mousePressed` and the callback method `mousePressed` carry the same name. in C++ it is not allowed to have a field or
  property and a method with the same name)
- `fill(float)` must be explicitly differentiated from `fill(uint32_t)` e.g `fill(1.0f)` or `fill(0xFF44FF88)` ( the
  same is true for `stroke(...)` ) @reason(in a different way from Java, C++ finds two methods that only differ in their
  parameter types i.e `float` and `uint32_t` ambiguous)

## Known Issues

- a LOT of functions + methods + strategies are not yet implemented (the goal is to implement these on demand).
- color system is fixed to range from `0.0 ... 1.0` and only works with RGB(A) and uses RGBA internally always
- only tested on macOS + Raspberry Pi OS + Windows 11 + Ubuntu. although theoretically the external libraries as well as
  the build system should be cross-platform ( i.e macOS, Windows and any UNIX-like system ) it may, however, require
  some tweaking.

### Setting up Homebrew on macOS

on some *clean* homebrew installations on macOS the environment variable `$LIBRARY_PATH` is not set or at least does not
include the
homebrew libraries. if so you may need to add the line `export LIBRARY_PATH=$LIBRARY_PATH:/usr/local/lib` to your
profile e.g in `~/.zshrc` in *zsh* shell. note, that other shell environments use other profile files and mechanisms e.g
*bash* uses `~/.bashrc`. find out which shell you are using by typing `echo $0`.

if you have NO idea what this all means you might just try the following lines ( as always without the `$` ;) ) for
*zsh*:

```
$ { echo -e "\n# set library path\n"; [ -n "$LIBRARY_PATH" ] && echo "export LIBRARY_PATH=/usr/local/lib:\"\$LIBRARY_PATH\"" || echo "export LIBRARY_PATH=/usr/local/lib"; } >> "$HOME/.zshrc"
$ source "$HOME/.zshrc"
```

this will set the `$LIBRARY_PATH` in your *zsh* profile file.

## Resources

- Umgebung [examples](https://github.com/dennisppaul/umgebung-examples)
- Additional [libraries](https://github.com/dennisppaul/umgebung-libraries)

examples and library examples assume that all repositories are located on the same level:

```
.
├── umgebung
├── umgebung-examples
└── umgebung-libraries
```
