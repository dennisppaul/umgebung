# Umgebung / Documentation

## Default Application Structure

```
umgebung-example-app
├── CMakeLists.txt
├── image.png
└── umgebung-example-app.cpp
```

the directory `‌umgebung-example-app` should contain all source, header and resources ( e.g images, fonts and moview ) files.

in this example `umgebung-example-app.cpp` is the main source file containing the *entry points* used by *Umgebung*:

```
#include "Umgebung.h"

class UmgebungApp : public PApplet {

    void settings() {}

    void setup() {}

    void draw() {}

    void audioblock(const float *input, float *output, unsigned long length) {}
};

PApplet *instance() {
    return new UmgebungApp();
}
```

the only required function is `PApplet *instance();` which is required to return an instance of the application. the application class must be derived from `PApplet`.

in order to compile the application a CMake script `CMakeLists.txt` must be supplied. the following is a script that may be used to compile the above example:

```
cmake_minimum_required(VERSION 3.12)

project(umgebung-example-app)                                  # set application name
set(UMGEBUNG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..") # set absolut path to umgebung library e.g `set(UMGEBUNG_PATH "<absolute/path/to/library>")`
link_directories("/usr/local/lib")                     # optional, can help to fix issues with Homebrew on macOS

set(DISABLE_GRAPHICS FALSE)
set(DISABLE_AUDIO FALSE)
set(DISABLE_VIDEO FALSE)

# add source + header files from this directory

file(GLOB SOURCE_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(".")
target_compile_definitions(${PROJECT_NAME} PRIVATE UMGEBUNG_WINDOW_TITLE="${PROJECT_NAME}") # set window title

# add umgebung

set(UMGEBUNG_APP ${PROJECT_NAME})
include("${UMGEBUNG_PATH}/CMakeLists.txt")
```

each application may have an individual name defined in `project(<name-of-application>)`.

it is required to set the variable `UMGEBUNG_PATH` ( e.g `‌set(UMGEBUNG_PATH "/Users/username/Documents/dev/umgebung/git/umgebung/")` ) which must contain the absolute path to the *Umgebung* library ( i.e the folder that contains e.g this document as well as the `include` and `src` folders of *Umgebung* ). note, that in the example CMake file above the CMake variable `${CMAKE_CURRENT_SOURCE_DIR}` is used to navigate relative to the location of the CMake file ( e.g helpful in the examples ).

the command `link_directories("/usr/local/lib")` can be used to fix a linker error on macOS ( e.g `ld: library 'glfw' not found` ). this error indicates that the global library is not set or not set properly ( i.e `echo $LIBRARY_PATH` returns an empty response or points to a folder that does not contain `libglfw.dylib` in this example ).

*Umgebung* allows to exclude certain modules ( e.g to run a headless setup ).

```
set(DISABLE_GRAPHICS FALSE) # removes capability to create windows
set(DISABLE_AUDIO FALSE)    # removes capability to access audio hardware
set(DISABLE_VIDEO FALSE)    # removes capability to playback movies
```

the section `# add source + header files from this directory` collects all `*.cpp` files, points the compiler to search this folder for header files ( i.e `*.h` files ) and sets the application window name via `‌UMGEBUNG_WINDOW_TITLE`.

the section `# add umgebung` must be last in the CMake file and includes *Umgebung* as a library.

## Build Example Application

the example application can be compiled in the following way. navigate the shell to the project directory:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./umgebung-example-app
```

if changes are made to `umgebung-example-app.cpp` ( or any other file in that folder ) it is enough to just run:

```
$ make ; ./umgebung-example-app
```
