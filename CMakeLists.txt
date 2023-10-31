cmake_minimum_required(VERSION 3.12)
project(Umgebung)

# find packages
find_package(glfw3 REQUIRED)
if (APPLE)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif ()

find_package(portaudio REQUIRED)

if (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
endif ()

# set source files
set(SOURCE_FILES
        Umgebung.cpp
        UmgebungDraw.cpp
        UmgebungFunctions.cpp
        UmgebungSketch.cpp
        TestSketch.cpp
        UmgebungPrint.cpp # @development
)

# create the executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# link against libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glfw portaudio)

# For macOS, link against the OpenGL framework
if (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenGL_LIBRARY})
endif ()

# set C++ standard + compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# @optional set compiler flags for additional warnings or optimizations
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-c++17-extensions")

# specify the output directory (optional)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)