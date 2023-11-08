cmake_minimum_required(VERSION 3.12)

project(umgebung-example-app)                       # set application name
set(UMGEBUNG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..") # set absolut path to umgebung library
#set(UMGEBUNG_PATH "<absolute/path/to/library>")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISABLE_GRAPHICS")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISABLE_AUDIO")

# grab all .cpp files from this directory
file(GLOB SOURCE_FILES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
include_directories(".")

target_compile_definitions(${PROJECT_NAME} PRIVATE UMGEBUNG_WINDOW_TITLE="${PROJECT_NAME}") # set window title

# compiler flags

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-c++17-extensions")

# add umgebung ( + portaudio + glfw3 + opengl )

set(UMGEBUNG_APP ${PROJECT_NAME})
include("${UMGEBUNG_PATH}/CMakeLists.txt")

# create application bundle
# 
# set(CPACK_GENERATOR "Bundle")
# set(CPACK_BUNDLE_NAME Umgebung)
# set(CPACK_BUNDLE_ICON Umgebung.icns)
# set(CPACK_BUNDLE_PLIST Info.plist)
# set(CPACK_BUNDLE_STARTUP_COMMAND umgebung-example-app)
# include(CPack)
