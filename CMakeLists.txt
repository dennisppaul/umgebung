cmake_minimum_required(VERSION 3.12)

message("")
message("+++ UMGEBUNG_APP  : ${UMGEBUNG_APP}")
message("+++ UMGEBUNG_PATH : ${UMGEBUNG_PATH}")
message("")

if (DISABLE_GRAPHICS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DISABLE_GRAPHICS)
    message("+++ DISABLE_GRAPHICS")
endif (DISABLE_GRAPHICS)

if (DISABLE_AUDIO)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DISABLE_AUDIO)
    message("+++ DISABLE_AUDIO")
endif (DISABLE_AUDIO)

if (DISABLE_VIDEO)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DISABLE_VIDEO)
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
### DETECT RPI                                                                                         ###
##########################################################################################################

if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" AND UNIX)
    set(RPI true)
else ()
    set(RPI false)
endif ()

if (RPI)
    message(STATUS "Compiling on RPI")
else ()
    message(STATUS "Not compiling on RPI")
endif ()

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


if (APPLE)
elseif (RPI)
    find_package(PkgConfig REQUIRED)
else ()
    warning("plattform not supported")
endif ()

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS)
    # GLFW + OpenGL + GLEW + FTGL
    if (APPLE)
        find_package(glfw3 REQUIRED)
        find_library(OpenGL_LIBRARY OpenGL)
        find_package(glew REQUIRED)
        find_package(ftgl REQUIRED)
    elseif (RPI)
        pkg_search_module(GLFW REQUIRED glfw3)
        find_package(glew REQUIRED)
        pkg_search_module(FTGL REQUIRED ftgl)
    else ()
        warning("plattform not supported")
    endif ()
    # FFMPEG
    if (NOT DISABLE_VIDEO)
        if (APPLE)
            find_package(ffmpeg REQUIRED)
        elseif (RPI)
            pkg_search_module(AVCODEC REQUIRED libavcodec)
            pkg_search_module(AVFORMAT REQUIRED libavformat)
            pkg_search_module(AVUTIL REQUIRED libavutil)
            pkg_search_module(SWSCALE REQUIRED libswscale)
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
    elseif (RPI)
        pkg_search_module(PORTAUDIO REQUIRED portaudio-2.0)
    else ()
        warning("plattform not supported")
    endif (APPLE)
endif (NOT DISABLE_AUDIO)

##########################################################################################################
### SUPRESS WARNINGS FOR APPLE                                                                         ###
##########################################################################################################

if (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
endif (APPLE)

##########################################################################################################
### INCLUDE + LINK LIBRARIES                                                                           ###
##########################################################################################################

# GRAPHICS + VIDEO
if (NOT DISABLE_GRAPHICS)
    # GLFW + OpenGL + GLEW + FTGL
    if (APPLE)
        target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenGL_LIBRARY})
        target_link_libraries(${PROJECT_NAME} PUBLIC glew)
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PUBLIC ftgl)
        target_include_directories(${PROJECT_NAME} PRIVATE ${FTGL_INCLUDE_DIRS})
    elseif (RPI)
        target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES})
        target_include_directories(${PROJECT_NAME} ${GLFW_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
        target_include_directories(${PROJECT_NAME} ${GLEW_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${FTGL_LIBRARIES})
        target_include_directories(${PROJECT_NAME} ${FTGL_INCLUDE_DIRS})
    else ()
        warning("plattform not supported")
    endif ()
    # FFMPEG
    if (NOT DISABLE_VIDEO)
        if (APPLE)
            target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES} ${SWSCALE_LIBRARIES})
            target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        elseif (RPI)
            target_link_libraries(${PROJECT_NAME} PUBLIC ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
            target_include_directories(${PROJECT_NAME} PRIVATE ${AVCODEC_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
        else ()
            warning("plattform not supported")
        endif ()
    endif (NOT DISABLE_VIDEO)
endif (NOT DISABLE_GRAPHICS)

# AUDIO
if (NOT DISABLE_AUDIO)
    # PORTAUDIO
    if (APPLE)
        target_link_libraries(${PROJECT_NAME} PUBLIC portaudio)
        target_include_directories(${PROJECT_NAME} PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
    elseif (RPI)
        target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_LIBRARIES})
        target_include_directories(${PROJECT_NAME} ${PORTAUDIO_INCLUDE_DIRS})
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
