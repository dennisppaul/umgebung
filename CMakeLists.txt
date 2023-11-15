cmake_minimum_required(VERSION 3.12)

message(STATUS "")
message(STATUS "UMGEBUNG_APP  : ${UMGEBUNG_APP}")
message(STATUS "UMGEBUNG_PATH : ${UMGEBUNG_PATH}")

##########################################################################################################
### CHECK SYSTEM TYPE                                                                                  ###
##########################################################################################################

if (APPLE)
    message(STATUS "SYSTEM        : macOS")
elseif (UNIX)
    message(STATUS "SYSTEM        : UNIX ( including RPI )")
elseif (WIN32)
    message(STATUS "SYSTEM        : Windows")
    message(WARNING "Warning: System is not supported")
else ()
    message(STATUS "SYSTEM        : (unknown system)")
    message(WARNING "Warning: System is not supported")
endif ()
message(STATUS)

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
### EN/DISABLE LIBRARIES                                                                               ###
##########################################################################################################

if (DISABLE_GRAPHICS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_GRAPHICS)
    message("+++ DISABLE_GRAPHICS")
endif (DISABLE_GRAPHICS)

if (DISABLE_AUDIO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_AUDIO)
    message("+++ DISABLE_AUDIO")
endif (DISABLE_AUDIO)

if (DISABLE_VIDEO)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DISABLE_VIDEO)
    message("+++ DISABLE_VIDEO")
endif (DISABLE_VIDEO)

if (NOT DEFINED UMGEBUNG_APP OR "${UMGEBUNG_APP}" STREQUAL "")
    message("UMGEBUNG_APP not set")
    project(umgebung-lib)
    set(UMGEBUNG_APP ${PROJECT_NAME})
    set(BUILD_STANDALONE true)
else ()
    project(${UMGEBUNG_APP})
    set(BUILD_STANDALONE false)
endif ()

if (NOT DEFINED UMGEBUNG_PATH OR "${UMGEBUNG_PATH}" STREQUAL "")
    message("UMGEBUNG_PATH not set")
    set(UMGEBUNG_PATH ".")
endif ()

# add source + header files
set(HEADER_DIR "include" "${UMGEBUNG_PATH}/src")
set(SOURCE_DIR "src")
file(GLOB UMGEBUNG_SOURCE_FILES
        "${UMGEBUNG_PATH}/${SOURCE_DIR}/*.cpp"
        "${UMGEBUNG_PATH}/${SOURCE_DIR}/ip/*.cpp"
        "${UMGEBUNG_PATH}/${SOURCE_DIR}/ip/posix/*.cpp"
        "${UMGEBUNG_PATH}/${SOURCE_DIR}/osc/*.cpp"
)
include_directories("${UMGEBUNG_PATH}/include")
include_directories("${UMGEBUNG_PATH}/include/ip")
include_directories("${UMGEBUNG_PATH}/include/osc")

##########################################################################################################
### CREATE EXECUTABLE                                                                                  ###
##########################################################################################################

if (BUILD_STANDALONE)
    add_executable(${PROJECT_NAME} ${UMGEBUNG_SOURCE_FILES})
else (BUILD_STANDALONE)
    target_sources(${PROJECT_NAME}
            PUBLIC
            ${UMGEBUNG_SOURCE_FILES}
    )
endif (BUILD_STANDALONE)

##########################################################################################################
### SUPRESS WARNINGS FOR APPLE                                                                         ###
##########################################################################################################

if (APPLE)
    if (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
    else (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${UMGEBUNG_PATH}/CMakeModules")
    endif (BUILD_STANDALONE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif (APPLE)

##########################################################################################################
### FIND PACKAGES + LIBRARIES                                                                          ###
##########################################################################################################

find_package(PkgConfig REQUIRED)

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS)
    # GLFW + OpenGL + GLEW + FTGL
    # TODO learn more about case-sensitive packae and library names in CMake on different systems
    pkg_search_module(GLFW REQUIRED glfw3)
    pkg_search_module(FTGL REQUIRED ftgl)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)
    if (APPLE)
        #        find_library(OpenGL_LIBRARY OpenGL)
    elseif (UNIX)
        #        find_package(OpenGL REQUIRED)
    elseif (WIN32)
        warning("plattform not supported")
    else ()
        warning("plattform not supported")
    endif ()

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        pkg_search_module(AVCODEC REQUIRED libavcodec)
        pkg_search_module(AVFORMAT REQUIRED libavformat)
        pkg_search_module(AVUTIL REQUIRED libavutil)
        pkg_search_module(SWSCALE REQUIRED libswscale)
        if (APPLE)
            #            find_package(ffmpeg REQUIRED)
        elseif (UNIX)
            #            pkg_search_module(AVCODEC REQUIRED libavcodec)
            #            pkg_search_module(AVFORMAT REQUIRED libavformat)
            #            pkg_search_module(AVUTIL REQUIRED libavutil)
            #            pkg_search_module(SWSCALE REQUIRED libswscale)
        elseif (WIN32)
            warning("plattform not supported")
        else ()
            warning("plattform not supported")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    # PORTAUDIO
    if (APPLE)
        find_package(PORTAUDIO REQUIRED)
    elseif (UNIX)
        pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)
    elseif (WIN32)
        warning("plattform not supported")
    else ()
        warning("plattform not supported")
    endif ()
endif (NOT DISABLE_AUDIO)

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

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS)
    # GLFW + OpenGL + GLEW + FTGL
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${GLFW_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FTGL_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${FTGL_INCLUDE_DIRS})
    if (APPLE)
        #        target_link_libraries(${PROJECT_NAME} ${OpenGL_LIBRARY})
        target_link_libraries(${PROJECT_NAME} glew)
        target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
    elseif (UNIX)
        #        target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
        target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
    elseif (WIN32)
        warning("plattform not supported")
    else ()
        warning("plattform not supported")
    endif ()

    # FFMPEG
    if (NOT DISABLE_VIDEO)
        target_link_libraries(${PROJECT_NAME} ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        if (APPLE)
            #            target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES} ${SWSCALE_LIBRARIES})
            #            target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        elseif (UNIX)
            #            target_link_libraries(${PROJECT_NAME} ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
            #            target_include_directories(${PROJECT_NAME} PUBLIC ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        elseif (WIN32)
            warning("plattform not supported")
        else ()
            warning("plattform not supported")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    # PORTAUDIO
    if (APPLE)
        target_link_libraries(${PROJECT_NAME} portaudio)
        target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    elseif (UNIX)
        target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
    elseif (WIN32)
        warning("plattform not supported")
    else ()
        warning("plattform not supported")
    endif ()
endif (NOT DISABLE_AUDIO)

## ncurses
# TODO implement curses for headless mode
#if (UNIX)
#    find_package(Curses REQUIRED)
#    include_directories(${CURSES_INCLUDE_DIR})
#    target_link_libraries(${PROJECT_NAME} PUBLIC ${CURSES_LIBRARIES})
#endif (UNIX)
#if (WIN32)
#    target_link_libraries(${PROJECT_NAME} PUBLIC pdcurses)
#endif (WIN32)

# includes
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR})

# set C++ standard + compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# @optional set compiler flags for additional warnings or optimizations
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-c++17-extensions")
