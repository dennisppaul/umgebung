cmake_minimum_required(VERSION 3.12)

project(umgebung-lib VERSION 1.0 LANGUAGES CXX)

message(STATUS "----------------------------------------")
message(STATUS "UMGEBUNG_APP  : ${PROJECT_NAME}")

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
if (APPLE)
    if (BREW_FOUND)
        execute_process(COMMAND brew --prefix
                OUTPUT_VARIABLE HOMEBREW_PREFIX
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(HOMEBREW_LIB_PATH "${HOMEBREW_PREFIX}/lib")
        message(STATUS "HOMEBREW LIBS : ${HOMEBREW_LIB_PATH}")
    else ()
        message(WARNING "HOMEBREW not found.")
    endif ()
endif (APPLE)

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

# Add library target
if (APPLE OR UNIX)
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/posix/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/osc/*.cpp
    )
elseif (WIN32)
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/win32/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/osc/*.cpp
    )
else ()
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ip/posix/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/osc/*.cpp
    )
endif ()
add_library(umgebung-lib ${UMGEBUNG_SOURCE_FILES})

# Specify include directories for the library
target_include_directories(umgebung-lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ip
        ${CMAKE_CURRENT_SOURCE_DIR}/include/osc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/midi
        ${CMAKE_CURRENT_SOURCE_DIR}/include/dr_libs
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ip>
        #        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/osc>
)

##########################################################################################################
### EN/DISABLE LIBRARIES                                                                               ###
##########################################################################################################

add_library(umgebung-lib-interface INTERFACE)

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
    target_compile_definitions(umgebung-lib-interface INTERFACE SYSTEM_WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SYSTEM_WIN32)
else ()
    message(STATUS "SYSTEM        : Undefined, Good Luck!")
    target_compile_definitions(umgebung-lib-interface INTERFACE SYSTEM_UNDEFINED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SYSTEM_UNDEFINED)
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
    pkg_search_module(FTGL REQUIRED ftgl)
    if (APPLE)
        pkg_search_module(SDL2 REQUIRED sdl2)
        find_package(OpenGL REQUIRED) # or `OPENGL`
    elseif (UNIX)
        pkg_search_module(SDL2 REQUIRED sdl2)
        find_package(OpenGL REQUIRED)
    elseif (WIN32)
        find_package(OpenGL REQUIRED)
        find_package(SDL2 REQUIRED)
    else ()
        message(STATUS "plattform may not be supported.")
        pkg_search_module(SDL2 REQUIRED sdl2)
        find_package(OpenGL REQUIRED)
    endif ()

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        pkg_search_module(AVCODEC REQUIRED libavcodec)
        pkg_search_module(AVFORMAT REQUIRED libavformat)
        pkg_search_module(AVUTIL REQUIRED libavutil)
        pkg_search_module(SWSCALE REQUIRED libswscale)
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
        else ()
            message(STATUS "plattform may not be supported ... assuming windows.")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    pkg_search_module(SDL2 REQUIRED sdl2)
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
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32 winmm)
elseif (APPLE)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
else ()
endif ()

# GRAPHICS + VIDEO + AUDIO
if (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
endif (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)

if (NOT DISABLE_GRAPHICS)
    # OpenGL + FTGL + GLEW
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FTGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${FTGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        target_link_libraries(${PROJECT_NAME} ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        if (APPLE)
        elseif (UNIX)
        elseif (WIN32)
            message(STATUS "plattform may not be supported")
        else ()
            message(STATUS "plattform may not be supported ... assuming windows.")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

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

function(add_umgebung_libs)
    # Check if umgebung-lib-interface target already exists
    if (NOT TARGET umgebung-lib-interface)
        message(FATAL_ERROR "umgebung-lib-interface target not found. Make sure it is defined before calling add_umgebung_libs.")
    endif ()

    # Check if umgebung-lib target already exists
    if (NOT TARGET umgebung-lib)
        message(FATAL_ERROR "umgebung-lib target not found. Make sure it is defined before calling add_umgebung_libs.")
    endif ()

    # Link the libraries if they exist
    target_link_libraries(${PROJECT_NAME} PRIVATE umgebung-lib-interface)
    target_link_libraries(${PROJECT_NAME} PRIVATE umgebung-lib)
endfunction()


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
message(STATUS "----------------------------------------")
