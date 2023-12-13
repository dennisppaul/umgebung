cmake_minimum_required(VERSION 3.12)

project(umgebung-lib VERSION 1.0 LANGUAGES CXX)

message(STATUS "")
message(STATUS "UMGEBUNG_APP  : ${UMGEBUNG_APP}")

#option(NOT USE_PORTAUDIO_GLFW "Use SDL for graphics + audio instead of GLFW + PortAudio" ON)
set(USE_PORTAUDIO_GLFW OFF)

#if (NOT DEFINED UMGEBUNG_APP OR "${UMGEBUNG_APP}" STREQUAL "")
#    message(STATUS "CHARACTER     : using umgebung standalone ( UMGEBUNG_APP not set )")
#    set(BUILD_STANDALONE ON)
#else ()
#    set(BUILD_STANDALONE OFF)
#endif ()

##########################################################################################################
### CHECK FOR HOMEBREW                                                                                 ###
##########################################################################################################
find_program(BREW_FOUND brew)
if (BREW_FOUND)
    execute_process(COMMAND brew --prefix
            OUTPUT_VARIABLE HOMEBREW_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(HOMEBREW_LIB_PATH "${HOMEBREW_PREFIX}/lib")
    message(STATUS "HOMEBREW LIBS : ${HOMEBREW_LIB_PATH}")
else ()
    message(WARNING "HOMEBREW not found.")
endif ()

##########################################################################################################
### SUPPLEMENT LIBRARY SEARCH PATH                                                                     ###
##########################################################################################################

# NOTE this does not work when executed after `add_executable` or `add_library` … which is exactly what happens :(
# if (APPLE)
#     # NOTE add default search path for libraries on macOS e.g for default Homebrew installation
#     link_directories("/usr/local/lib")
#     link_directories("/opt/homebrew/lib")
#     message(STATUS "GLOBAL LIB    : adding `/usr/local/lib` + `/opt/homebrew/lib` to library search path")
# elseif (UNIX)
# elseif (WIN32)
# else ()
# endif ()

##########################################################################################################
### ADD SOURCE + HEADER FILES                                                                          ###
##########################################################################################################

# Add the library target
file(GLOB_RECURSE UMGEBUNG_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        #    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp>
)
add_library(umgebung-lib ${UMGEBUNG_SOURCE_FILES})

# Specify include directories for the library
target_include_directories(umgebung-lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ip
        ${CMAKE_CURRENT_SOURCE_DIR}/include/osc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/midi
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ip>
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/osc>
)

##########################################################################################################
### EN/DISABLE LIBRARIES                                                                               ###
##########################################################################################################

add_library(umgebung-lib-interface INTERFACE)

if (USE_PORTAUDIO_GLFW)
    message(STATUS "FRAMEWORKS    : using GLFW + PortAudio")
    target_compile_definitions(umgebung-lib-interface INTERFACE USE_PORTAUDIO_GLFW)
    target_compile_definitions(${PROJECT_NAME} PUBLIC USE_PORTAUDIO_GLFW)
else (USE_PORTAUDIO_GLFW)
    message(STATUS "FRAMEWORKS    : using SDL ( graphics + audio )")
endif ()

if (DISABLE_GRAPHICS)
    target_compile_definitions(umgebung-lib-interface INTERFACE DISABLE_GRAPHICS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_GRAPHICS)
    message(STATUS "DISABLING     : graphics")
endif (DISABLE_GRAPHICS)

if (DISABLE_AUDIO)
    target_compile_definitions(umgebung-lib-interface INTERFACE DISABLE_AUDIO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_AUDIO)
    message(STATUS "DISABLING     : audio")
endif (DISABLE_AUDIO)

if (DISABLE_VIDEO)
    target_compile_definitions(umgebung-lib-interface INTERFACE DISABLE_VIDEO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_VIDEO)
    message(STATUS "DISABLING     : video")
endif (DISABLE_VIDEO)

##########################################################################################################
### CREATE EXECUTABLE                                                                                  ###
##########################################################################################################

#if (BUILD_STANDALONE)
#    add_executable(${PROJECT_NAME} ${UMGEBUNG_SOURCE_FILES})
#    target_compile_definitions(${PROJECT_NAME} PUBLIC UMGEBUNG_STANDALONE)
#else (BUILD_STANDALONE)
target_sources(${PROJECT_NAME}
        PUBLIC
        ${UMGEBUNG_SOURCE_FILES}
)
#endif (BUILD_STANDALONE)

##########################################################################################################
### CHECK SYSTEM TYPE                                                                                  ###
##########################################################################################################

if (APPLE)
    message(STATUS "SYSTEM        : macOS")
    target_compile_definitions(umgebung-lib-interface INTERFACE SYSTEM_MACOS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SYSTEM_MACOS)
elseif (UNIX)
    message(STATUS "SYSTEM        : UNIX ( including RPI )")
    target_compile_definitions(umgebung-lib-interface INTERFACE SYSTEM_UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SYSTEM_UNIX)
elseif (WIN32)
    message(STATUS "SYSTEM        : Windows")
    message(WARNING "Warning: System is not supported")
    target_compile_definitions(umgebung-lib-interface INTERFACE SYSTEM_WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SYSTEM_WIN32)
else ()
    message(STATUS "SYSTEM        : (unknown system)")
    message(WARNING "Warning: System is not supported")
endif ()

# NOTE removed specific system check for now ( because it fails with ninja build system ). it may not be necessary to have such a specific check for RPI.
#try_run(RUN_RESULT_VAR COMPILE_RESULT_VAR
#        ${UMGEBUNG_PATH}/tools/system_check
#        ${UMGEBUNG_PATH}/tools/system_check.cpp
#        RUN_OUTPUT_VARIABLE SYSTEM_CHECK_OUTPUT)
#
#if(RUN_RESULT_VAR EQUAL 1)
#    set(IS_WINDOWS ON)
#    message(STATUS "Running on Windows")
#elseif(RUN_RESULT_VAR EQUAL 2)
#    set(IS_MACOS ON)
#    message(STATUS "Running on macOS")
#elseif(RUN_RESULT_VAR EQUAL 3)
#    set(IS_RASPBERRY_PI ON)
#    message(STATUS "Running on Raspberry Pi")
#elseif(RUN_RESULT_VAR EQUAL 4)
#    set(IS_LINUX ON)
#    message(STATUS "Running on Linux (non-Raspberry Pi)")
#else()
#    set(IS_UNKNOWN_SYSTEM ON)
#    message(STATUS "Running on an unknown system")
#endif()

##########################################################################################################
### SUPRESS WARNINGS FOR APPLE                                                                         ###
##########################################################################################################

if (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif (APPLE)

##########################################################################################################
### FIND PACKAGES + LIBRARIES                                                                          ###
##########################################################################################################

find_package(PkgConfig REQUIRED)
if (WIN32)
elseif (APPLE)
elseif (UNIX)
    find_package(Threads REQUIRED)
else ()
endif ()

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS)
    # OpenGL + GLEW + FTGL
    pkg_search_module(GLEW REQUIRED glew)
    find_package(OpenGL REQUIRED) # or `OPENGL`
    pkg_search_module(FTGL REQUIRED ftgl)
    if (NOT USE_PORTAUDIO_GLFW)
        pkg_search_module(SDL2 REQUIRED sdl2)
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    else (NOT USE_PORTAUDIO_GLFW)
        # GLFW
        # TODO learn more about case-sensitive packae and library names in CMake on different systems
        pkg_search_module(GLFW REQUIRED glfw3)
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT USE_PORTAUDIO_GLFW)

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        pkg_search_module(AVCODEC REQUIRED libavcodec)
        pkg_search_module(AVFORMAT REQUIRED libavformat)
        pkg_search_module(AVUTIL REQUIRED libavutil)
        pkg_search_module(SWSCALE REQUIRED libswscale)
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    if (NOT USE_PORTAUDIO_GLFW)
        pkg_search_module(SDL2 REQUIRED sdl2)
    else (NOT USE_PORTAUDIO_GLFW)
        # PORTAUDIO
        pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT USE_PORTAUDIO_GLFW)
endif (NOT DISABLE_AUDIO)

# MIDI
if (NOT DISABLE_MIDI)
    pkg_search_module(RTMIDI REQUIRED rtmidi)
endif (NOT DISABLE_MIDI)

##########################################################################################################
### SUPRESS WARNINGS FOR APPLE                                                                         ###
##########################################################################################################

if (WIN32)
elseif (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
elseif (UNIX)
else ()
endif ()

##########################################################################################################
### INCLUDE + LINK LIBRARIES                                                                           ###
##########################################################################################################

if (HOMEBREW_LIB_PATH)
    target_link_directories(${PROJECT_NAME} PUBLIC ${HOMEBREW_LIB_PATH})
endif ()

if (WIN32)
elseif (APPLE)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
else ()
endif ()

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)
    if (NOT USE_PORTAUDIO_GLFW)
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
    endif (NOT USE_PORTAUDIO_GLFW)
endif (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)

if (NOT DISABLE_GRAPHICS)
    # OpenGL + FTGL + GLEW
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FTGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${FTGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})

    if (NOT USE_PORTAUDIO_GLFW)
    else (NOT USE_PORTAUDIO_GLFW)
        # GLFW
        target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT USE_PORTAUDIO_GLFW)

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        target_link_libraries(${PROJECT_NAME} ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    if (NOT USE_PORTAUDIO_GLFW)
    else (NOT USE_PORTAUDIO_GLFW)
        # PORTAUDIO
        if (APPLE)
            target_link_libraries(${PROJECT_NAME} portaudio)
            target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
        elseif (UNIX)
            target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_LIBRARIES})
            target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported")
        endif ()
    endif (NOT USE_PORTAUDIO_GLFW)
endif (NOT DISABLE_AUDIO)

# MIDI
if (NOT DISABLE_MIDI)
    target_link_libraries(${PROJECT_NAME} ${RTMIDI_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${RTMIDI_INCLUDE_DIRS})
    add_definitions(${RTMIDI_CFLAGS_OTHER})
    #    target_compile_definitions(${PROJECT_NAME} PUBLIC ${RTMIDI_CFLAGS_OTHER})
endif (NOT DISABLE_MIDI)

##########################################################################################################
### COMPILER FLAGS                                                                                     ###
##########################################################################################################

# TODO check if this is necessary or if they can be inherited from the parent project
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++17-extensions")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

# TODO implement curses for headless mode
## ncurses
#if (UNIX)
#    find_package(Curses REQUIRED)
#    include_directories(${CURSES_INCLUDE_DIR})
#    target_link_libraries(${PROJECT_NAME} PUBLIC ${CURSES_LIBRARIES})
#endif (UNIX)
#if (WIN32)
#    target_link_libraries(${PROJECT_NAME} PUBLIC pdcurses)
#endif (WIN32)