cmake_minimum_required(VERSION 3.12)

project(umgebung-with-OSC)                             # set application name
set(UMGEBUNG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..") # set absolut path to umgebung library
#set(UMGEBUNG_PATH "<absolute/path/to/library>")
set(DISABLE_GRAPHICS FALSE)
set(DISABLE_AUDIO TRUE)
set(DISABLE_VIDEO TRUE)

# add source + header files from this directory

file(GLOB SOURCE_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(".")
target_compile_definitions(${PROJECT_NAME} PRIVATE UMGEBUNG_WINDOW_TITLE="${PROJECT_NAME}") # set window title

# add umgebung

set(UMGEBUNG_APP ${PROJECT_NAME})
include("${UMGEBUNG_PATH}/CMakeLists.txt")
