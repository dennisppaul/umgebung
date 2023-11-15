cmake_minimum_required(VERSION 3.12)

project(umgebung-app)                                  # set application name
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

# create application bundle
# 
# set(CPACK_GENERATOR "Bundle")
# set(CPACK_BUNDLE_NAME Umgebung)
# set(CPACK_BUNDLE_ICON Umgebung.icns)
# set(CPACK_BUNDLE_PLIST Info.plist)
# set(CPACK_BUNDLE_STARTUP_COMMAND umgebung-app)
# include(CPack)
