# Umgebung

*Umgebung* is a lightweight C++ environment for small audio and graphics based applications. it is inspired by [Processing.org](https://processing.org) and similar environments.

⚠️ *Umgebung* is a *Work in Progress* (WIP) ⚠️ please always keep this in mind. if you find a bug, an issue, something missing, have an idea for improvements or a feature request, please use GitHub’s [Issue Tracking System](https://github.com/dennisppaul/umgebung/issues).

## Features

- processing-style API in C/C++
- simple structure ( e.g `setup()` and `draw()` )
- shape-based drawing (e.g `line()`, `ellipse()`, `beginShape()`…)
- cross-platform ( macOS, Linux, Raspberry Pi, Windows and potentially iOS, Android, WebGL etcetera )
- simple application structure `.cpp` file + `CMakeLists.txt`
- based on CMake build system
- native access to *real* APIs e.g OpenGL, FFmpeg, PortAudio, and others
- small, portable builds

## Quickstart

```sh
git clone --recurse-submodules https://github.com/dennisppaul/umgebung
git clone --recurse-submodules https://github.com/dennisppaul/umgebung-examples
cd umgebung
./install-macOS.sh # or ./install-linux-apt.sh
cd ../umgebuncdg-examples/Basics/minimal
cmake -B build ; cmake --build build ; ./build/minimal
```

( on some linux systems you might need to installed `git` first with `sudo apt install -y git` )

## Why *Umgebung*?

well, *Umgebung* is actually imitating [Processing.org](https://processing.org) quite bluntly ;) or a compliment to the original [Processing.org](https://processing.org) project, or a token of respect.

this project aims to supply a framework that allows writing applications that more or less look like original [Processing.org](https://processing.org) sketches.

*Umgebung* may also function as a statement on what is good and valuable about the [Processing.org](https://processing.org) *idiom* while at the same time suggesting disconnecting it from one specific group, application or project … maybe a bit like markdown in that sense.

*Umgebung* is yet another programming environment for designers, makers, and artists. so why does it exist? and why not use [Processing.org](https://processing.org), [OpenFrameworks](https://openframeworks.cc), [Cinder](https://libcinder.org) or any of the other frameworks out there?

well, the main reason why we even created *Umgebung* was basically to have a C++ derivative of Processing.org that stays very close to the *original* idiom. while Cinder and OpenFrameworks, for example, are fantastic projects, they differ quite a lot from the *Processing.org Idiom* ( i.e a `setup()-draw()` structure, `stroke()+fill()` and shape-based drawing, etcetera ). *Umgebung*, however, literally imitates core Processing classes and functions.

why not use the original Java-based *Processing.org* then? there are a few key aspects that just cannot and probably will never be resolved due to limitations imposed by Java. while Java is a very well-designed language and does many things better than C/C++, it has some drawbacks.

the most important benefit of moving to C/C++ is the availability of many extremely powerful and widespread libraries and APIs ( e.g OpenGL, OpenCV, FFmpeg, PortAudio ) that can be natively used and integrated into applications and sketches without the need for a native binding library. while many of these libraries have been fully or partly made accessible in Processing through native bindings, oftentimes these libraries or their bindings introduce significant overhead ( technically, administratively, etcetera ), are not at the current version, are incompatible with newer versions of OSes, or do not expose all functionality ( e.g unstable webcam support on macOS ). while this also happens with C/C++ libraries and APIs, the problems are greatly reduced. also, there is still a significant number of libraries and APIs not available for Java-based Processing or that are impossible to port because of limitations posed by Java VM implementations ( e.g multi-channel audio ).

in addition to this, due to the nature of C/C++ and the way *Umgebung* is implemented, applications and sketches developed with *Umgebung* can be built and deployed with a very(!) small memory and CPU footprint. this means that applications and sketches can potentially run on smaller or older hardware like, e.g Raspberry Pi. some modules can even be excluded from a build to reduce the footprint even more. *Umgebung* can even run *truly* headless ( i.e it does not require any virtual offscreen graphics driver if no window is required ).

furthermore, *Umgebung* uses CMake as a build system. CMake is very well-designed and more or less easy to learn and extend. this also means that *Umgebung* applications and sketches can be developed in more or less any text editor or IDE ( e.g Visual Studio Code, Zed, CLion, Vim ). it does not require proprietary build systems like Xcode.

and finally, although C and especially C++ is known for its horrific errors, confusing error messages, and volatile behavior at times, with the advent of LLM-based programming assistants, the entry threshold can be greatly reduced. trust the process. PS ( if *vibe coding* is even a thing this should be easy ;) )

PS note the character of this project is that features will be added as they are needed. it might partly replicate but not fully emulate the original [Processing.org](https://processing.org) environment.

see [DOCUMENTATION](documentation/DOCUMENTATION.md) for usage information and [examples](https://github.com/dennisppaul/umgebung-examples) for applications.

some relevant *Umgebung* repositories are:

- [Umgebung](https://github.com/dennisppaul/umgebung) :: this repository, containing the *Umgebung* library
- [Umgebung Example](https://github.com/dennisppaul/umgebung-examples) :: a repository containing all the examples
- [Umgebung Libraries](https://github.com/dennisppaul/umgebung-libraries) :: a repository containing some libraries ( e.g Dear ImGui ) to extend *Umgebung*
- [Umgebung Arduino](https://github.com/dennisppaul/umgebung-arduino) :: an ( experimental ) *board definition* to run *Umgebung* applications from Arduino IDE or `arduino-cli`

## Credits

this project relies on the following packages:

- [CMake](https://cmake.org/) `@version(macOS, 3.31.6)`
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) `@version(macOS, 2.4.3)`
- [SDL3](https://www.libsdl.org) `@version(macOS, 3.2.8)`
- [GLEW](https://glew.sourceforge.net/) `@version(macOS, 2.2.0)`
- [HarfBuzz](https://github.com/harfbuzz/harfbuzz) `@version(macOS, 11.0.0)`
- [Freetype](https://www.freetype.org/) `@version(macOS, 2.13.3)`
- [ffmpeg](https://ffmpeg.org) `@version(macOS, 7.1.1)`
- [rtmidi](https://github.com/thestk/rtmidi) `@version(macOS, 6.0.0)`
- [glm](https://glm.g-truc.net/) `@version(macOS, 1.0.1)`
- [dylibbundler](https://github.com/auriamg/macdylibbundler) ( only required for bundling standalone macOS applications. it can also be installed via homebrew with `brew install dylibbundler` ) `@version(macOS, 1.0.5)`
- [PortAudio](https://www.portaudio.com) ( can be used as a replacement for SDL-based audio. in contrast to SDL it can output audio to multiple channels. it can also be installed via homebrew with `brew install portaudio` ) `@version(macOS, 19.7.0)`
- [oscpack](http://www.rossbencina.com/code/oscpack) ( included as source files )
- [dr_libs](https://github.com/mackron/dr_libs) ( included as source files )
- [SimplexNoise](https://github.com/SRombauts/SimplexNoise) ( included as source files )
- [stb_image + stb_image_write](https://github.com/nothings/stb) ( included as source files )
- [tiny_obj_load](https://github.com/tinyobjloader/tinyobjloader) ( included as source files )
- [tinyfiledialogs](https://sourceforge.net/projects/tinyfiledialogs/) ( included as source files )
- [PolyPartition](https://github.com/ivanfratric/polypartition) ( included as source files )
- [earcut.hpp](https://github.com/mapbox/earcut.hpp) ( included as source files )
- [libtess2](https://github.com/memononen/libtess2) ( included as source files )

( `@version(macOS, X.X.X)` states the platform and version number of the library with which *Umgebung* has been tested on macOS. other versions might work as well but are not guaranteed to. )

and as mentioned before of course [Processing.org](https://processing.org)