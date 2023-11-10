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

# create the executable
if (BUILD_STANDALONE)
    add_executable(${PROJECT_NAME} ${UMGEBUNG_SOURCE_FILES})
    #    add_library(${PROJECT_NAME} SHARED ${UMGEBUNG_SOURCE_FILES})
else (BUILD_STANDALONE)
    target_sources(${PROJECT_NAME}
            PUBLIC
            ${UMGEBUNG_SOURCE_FILES}
    )
endif (BUILD_STANDALONE)

# glfw
if (NOT DISABLE_GRAPHICS)
    find_package(glfw3 REQUIRED)
    if (APPLE)
        find_library(OpenGL_LIBRARY OpenGL)
        target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenGL_LIBRARY})
    endif ()
    target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
endif (NOT DISABLE_GRAPHICS)

# portaudio + ftgl + glew
if (APPLE)
    if (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
    else (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${UMGEBUNG_PATH}/CMakeModules")
    endif (BUILD_STANDALONE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif (APPLE)

if (NOT DISABLE_AUDIO)
    find_package(portaudio REQUIRED)
endif (NOT DISABLE_AUDIO)

if (NOT DISABLE_GRAPHICS)
    find_package(ftgl REQUIRED)
    find_package(glew REQUIRED)
#     find_package(ffmpeg REQUIRED)
endif (NOT DISABLE_GRAPHICS)

if (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
endif (APPLE)

## @development adding portaudio + ftgl manually as it seems to cause problems on some MacOS machines TODO: fix this
#set(USE_MANUAL_CONFIGURATION_FOR_LIBRARIES false)
#if (USE_MANUAL_CONFIGURATION_FOR_LIBRARIES)
#    message("USE_MANUAL_CONFIGURATION_FOR_LIBRARIES")
#    target_link_libraries(${PROJECT_NAME} PRIVATE ${PORTAUDIO_LIBRARIES})
#    target_include_directories(${PROJECT_NAME} PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
#    target_link_libraries(${PROJECT_NAME} PRIVATE ${FTGL_LIBRARIES})
#    target_include_directories(${PROJECT_NAME} PRIVATE ${FTGL_INCLUDE_DIRS})
#else (USE_MANUAL_CONFIGURATION_FOR_LIBRARIES)
#message("not using USE_MANUAL_CONFIGURATION_FOR_LIBRARIES")
if (NOT DISABLE_AUDIO)
    target_link_libraries(${PROJECT_NAME} PUBLIC portaudio)
    target_include_directories(${PROJECT_NAME} PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
endif (NOT DISABLE_AUDIO)
if (NOT DISABLE_GRAPHICS)
    target_link_libraries(${PROJECT_NAME} PUBLIC ftgl)
    target_include_directories(${PROJECT_NAME} PRIVATE ${FTGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PUBLIC glew)
#     target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES})
#     target_include_directories(${PROJECT_NAME} PRIVATE ${FFMPEG_INCLUDE_DIRS})
endif (NOT DISABLE_GRAPHICS)
#endif (USE_MANUAL_CONFIGURATION_FOR_LIBRARIES)

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
