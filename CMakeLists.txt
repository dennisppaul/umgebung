cmake_minimum_required(VERSION 3.20)

project(umgebung-lib)

#option(SET_COMPILER_FLAGS "Set Compiler Flags to C++17" OFF)

message(STATUS "----------------------------------------")
message(STATUS "UMGEBUNG_APP  : ${PROJECT_NAME}")

option(ENABLE_PORTAUDIO "Enable PortAudio output" ON)

#if (NOT DEFINED UMGEBUNG_APP OR "${UMGEBUNG_APP}" STREQUAL "")
#    message(STATUS "CHARACTER     : using umgebung standalone ( UMGEBUNG_APP not set )")
#    set(BUILD_STANDALONE ON)
#else ()
#    set(BUILD_STANDALONE OFF)
#endif ()

##########################################################################################################
### COMPILER FLAGS                                                                                     ###
##########################################################################################################
#function(set_umgebung_compiler_flags)
#    set(CMAKE_CXX_STANDARD 17 PARENT_SCOPE)
#    set(CMAKE_CXX_STANDARD_REQUIRED ON PARENT_SCOPE)
##    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION" PARENT_SCOPE)
#    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)
#endfunction()

# TODO somehow this does not work?!?
## NOTE can and should be set in root cmake script
#function(set_umgebung_compiler_flags)
#    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
#    set(CMAKE_CXX_STANDARD 17)
#    set(CMAKE_CXX_STANDARD_REQUIRED ON)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
#    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++17-extensions")
#    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
#    #set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X")
#endfunction()

#if (SET_COMPILER_FLAGS)
#    message(STATUS "COMPILER_FLAGS: Umgebung is setting compiler flags")
#    # TODO check if this is necessary or if they can be inherited from the parent project
#    # TODO does not work if called via function … investigate
##    set_umgebung_compiler_flags()
#    set(CMAKE_CXX_STANDARD 17)
#    set(CMAKE_CXX_STANDARD_REQUIRED ON)
#    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X")
#else (SET_COMPILER_FLAGS)
#    message(STATUS "COMPILER_FLAGS: Umgebung is expecting compiler flags to be set by root script")
#endif (SET_COMPILER_FLAGS)

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
if (APPLE)
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.mm
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/libtess2/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/clipper/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/posix/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/osc/*.cpp
    )
elseif (UNIX)
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/libtess2/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/clipper/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/posix/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/osc/*.cpp
    )
elseif (WIN32)
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/libtess2/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/clipper/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/win32/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/osc/*.cpp
    )
else ()
    file(GLOB UMGEBUNG_SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/libtess2/*.c
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/clipper/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/ip/posix/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/libraries/osc/*.cpp
    )
endif ()

if (NOT TARGET umgebung-lib)
    add_library(umgebung-lib ${UMGEBUNG_SOURCE_FILES})
endif ()
#target_compile_features(umgebung-lib PUBLIC cxx_std_17)
#target_compile_options(umgebung-lib PUBLIC -std=c++17)

# Specify include directories for the library
target_include_directories(umgebung-lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/shaders
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libraries
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libraries/dr_libs
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libraries/ip
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libraries/libtess2
        ${CMAKE_CURRENT_SOURCE_DIR}/include/libraries/osc
)

##########################################################################################################
### EN/DISABLE LIBRARIES                                                                               ###
##########################################################################################################
if (NOT TARGET umgebung-lib-interface)
    add_library(umgebung-lib-interface INTERFACE) # this enables `glm::aligned_vec*` types
endif ()

target_compile_definitions(${PROJECT_NAME} PUBLIC GLM_ENABLE_EXPERIMENTAL)

if (NOT DISABLE_MAIN)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SDL_MAIN_USE_CALLBACKS)
else ()
    message(STATUS "DISABLE_MAIN  : no `main` function entry point. must be defined in project.")
endif (NOT DISABLE_MAIN)

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

if (ENABLE_PORTAUDIO)
    target_compile_definitions(umgebung-lib-interface INTERFACE ENABLE_PORTAUDIO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_PORTAUDIO)
    message(STATUS "ENABLING      : PortAudio Driver")
endif (ENABLE_PORTAUDIO)

if (ENABLE_CAPTURE)
    target_compile_definitions(umgebung-lib-interface INTERFACE ENABLE_CAPTURE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ENABLE_CAPTURE)
    message(STATUS "ENABLING      : Capture")
endif (ENABLE_CAPTURE)

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
    # OpenGL + GLEW
    pkg_search_module(GLEW REQUIRED glew)
    pkg_search_module(GLM REQUIRED glm) # TODO add to brew file
    find_package(OpenGL REQUIRED)
    find_package(SDL3 CONFIG REQUIRED)
    pkg_search_module(HARFBUZZ REQUIRED harfbuzz)
    pkg_check_modules(FREETYPE REQUIRED freetype2)

    if (APPLE)
    elseif (UNIX)
    elseif (WIN32)
    else ()
        message(STATUS "plattform may not be supported.")
    endif ()

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        pkg_check_modules(FFMPEG REQUIRED
                libavcodec
                libavformat
                libavutil
                libswscale
                libswresample
                libavdevice
        )
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
    if (ENABLE_PORTAUDIO)
        pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)
    else (ENABLE_PORTAUDIO)
        find_package(SDL3 CONFIG REQUIRED)
    endif (ENABLE_PORTAUDIO)
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
    target_link_libraries(${PROJECT_NAME} PUBLIC wsock32 ws2_32 winmm)
elseif (APPLE)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
else ()
endif ()

# GRAPHICS + VIDEO + AUDIO
if (NOT DISABLE_AUDIO)
    # TODO always add portaudio?!?
    target_link_libraries(${PROJECT_NAME} PUBLIC ${PORTAUDIO_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
endif (NOT DISABLE_AUDIO)

if (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${HARFBUZZ_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${HARFBUZZ_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FREETYPE_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3) # NOTE not sure about these explicit lib names
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})
endif (NOT DISABLE_GRAPHICS OR NOT DISABLE_AUDIO)

if (NOT DISABLE_GRAPHICS)
    # OpenGL + GLEW + GLM
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${GLEW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${GLM_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIRS})

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES})
        if (APPLE)
            target_link_libraries(${PROJECT_NAME} PUBLIC
                    "-framework AVFoundation"
                    "-framework Foundation"
                    "-framework CoreMedia"
            )
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
    target_link_libraries(${PROJECT_NAME} PUBLIC ${RTMIDI_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${RTMIDI_INCLUDE_DIRS})
    # add_definitions(${RTMIDI_CFLAGS_OTHER})
    # message(STATUS "RTMIDI_CFLAGS_OTHER: ${RTMIDI_CFLAGS_OTHER}")
    foreach(flag ${RTMIDI_CFLAGS_OTHER})
        if (flag MATCHES "^-D[^=]+(=.+)?$")  # only pass valid -D macros
            target_compile_definitions(${PROJECT_NAME} PUBLIC "${flag}")
        elseif (flag STREQUAL "-pthread")
            target_compile_options(${PROJECT_NAME} PUBLIC "${flag}")
        endif ()
    endforeach ()
    # target_compile_definitions(${PROJECT_NAME} PUBLIC ${RTMIDI_CFLAGS_OTHER})
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
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(${PROJECT_NAME} PRIVATE umgebung-lib-interface)
    target_link_libraries(${PROJECT_NAME} PRIVATE umgebung-lib)
endfunction()


##########################################################################################################
### TODO                                                                                               ###
##########################################################################################################

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

##########################################################################################################
### NOTE                                                                                               ###
##########################################################################################################

# use these in projects:
# - prepare umgebung ( before `add_executable` ): `include(${UMGEBUNG_PATH}/cmake/default-pre.cmake)`
# - prepare umgebung ( after `add_executable` ): `include(${UMGEBUNG_PATH}/cmake/default-post.cmake)`

##########################################################################################################
### TEST                                                                                               ###
##########################################################################################################

option(BUILD_TESTING "Enable tests" OFF)

if (BUILD_TESTING)
    message(STATUS "TESTING       : enabled")

    enable_testing()

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for Mac OS X")

    add_executable(umgebung-tests test/umgebung_test.cpp)
    target_link_libraries(umgebung-tests umgebung-lib umgebung-lib-interface)

    add_test(NAME UmgebungTest COMMAND umgebung-tests)
else ()
endif ()

# run test from CLI with `cmake -B build -DBUILD_TESTING=ON ; cmake --build build ; ctest --test-dir build`

##########################################################################################################
### END OF SCRIPT                                                                                      ###
##########################################################################################################

message(STATUS "----------------------------------------")
