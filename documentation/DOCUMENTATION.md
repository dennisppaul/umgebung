# Umgebung / Documentation

## Default Application Structure

```
umgebung-example-app
├── CMakeLists.txt
├── image.png
└── umgebung-example-app.cpp
```

the directory `umgebung-example-app` should contain all source, header and resources ( e.g images, fonts and moview )
files.

in this example `umgebung-example-app.cpp` is the main source file containing the *entry points* used by *Umgebung*:

```
#include "Umgebung.h"

void settings() {}

void setup() {}

void draw() {}

```

in order to compile the application a CMake script `CMakeLists.txt` must be supplied. the following is a script that may
be used to compile the above example:

```cmake
cmake_minimum_required(VERSION 3.12)

project(umgebung-example-app)                                      # set application name
set(UMGEBUNG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../umgebung") # set path to umgebung library

# --------- no need to change anything below this line ------------

set(CMAKE_CXX_STANDARD 17)                                         # set c++ standard, this needs to happen before `add_executable`
set(CMAKE_CXX_STANDARD_REQUIRED ON)                                # minimum is C++17 but 20 and 23 should also be fine

include_directories(".")                                           # add all `.h` header files from this directory
file(GLOB SOURCE_FILES "*.cpp")                                    # collect all `.cpp` source files from this directory
add_executable(${PROJECT_NAME} ${SOURCE_FILES})                    # add source files to application

add_subdirectory(${UMGEBUNG_PATH} ${CMAKE_BINARY_DIR}/umgebung-lib-${PROJECT_NAME}) # add umgebung location
add_umgebung_libs()                                                # add umgebung library
```

each application may have an individual name defined in `project(<name-of-application>)`.

it is required to set the variable `UMGEBUNG_PATH` ( e.g `set(UMGEBUNG_PATH "/Users/username/Documents/dev/umgebung/git/umgebung/")` ) which must contain the absolute path to the *Umgebung* library ( i.e the folder that contains e.g this document as well as the `include` and `src` folders of *Umgebung* ). note, that in the example CMake file above the CMake variable `${CMAKE_CURRENT_SOURCE_DIR}` is used to navigate relative to the location of the CMake file ( e.g helpful in the examples ).

the command `link_directories("/usr/local/lib")` can be used to fix a linker error on macOS ( e.g
`ld: library 'glfw' not found` ). this error indicates that the global library is not set or not set properly ( i.e `echo $LIBRARY_PATH` returns an empty response or points to a folder that does not contain `libglfw.dylib` in this example ).

the section `# add umgebung` must be last in the CMake file and includes *Umgebung* as a library.

## Build Example Application

to compile and run this example just type the following commands in the terminal:

```sh
$ cmake -B build      # prepare build system
$ cmake --build build # compile the application
$ ./build/minimal     # to run the application
```

there is also a way to concatenate all three commands in one line like this:

```sh
$ cmake -B build ; cmake --build build ; ./build/minimal
```

to start a *clean* build simply delete the build directory:

```sh
$ rm -rf build
```

if changes are made to `umgebung-example-app.cpp` ( or any other source or header file in that folder ) it is enough to just run:

```
$ cmake --build build
```
