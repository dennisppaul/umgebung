cmake_minimum_required(VERSION 3.12)

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

message("")
message("+++ UMGEBUNG_APP  : ${UMGEBUNG_APP}")
message("+++ UMGEBUNG_PATH : ${UMGEBUNG_PATH}")
message("")

# SOURCE FILES
set(HEADER_DIR "include" "${UMGEBUNG_PATH}/src")
set(SOURCE_DIR "src")
file(GLOB UMGEBUNG_SOURCE_FILES "${UMGEBUNG_PATH}/${SOURCE_DIR}/*.cpp")
#file(GLOB UMGEBUNG_SOURCE_FILES "${SOURCE_DIR}/*.cpp")
include_directories("${UMGEBUNG_PATH}/include")

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
find_package(glfw3 REQUIRED)
if (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenGL_LIBRARY})
endif ()
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# portaudio
if (APPLE)
    if (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")
    else (BUILD_STANDALONE)
        list(APPEND CMAKE_MODULE_PATH "${UMGEBUNG_PATH}/CMakeModules")
    endif (BUILD_STANDALONE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
endif (APPLE)
find_package(portaudio REQUIRED)
if (APPLE)
    set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS OFF)
endif (APPLE)

# link against libraries
# @development adding portaudio manually as it seems to cause problems on some MacOS machines TODO: fix this
# replaced `portaudio` with `${PORTAUDIO_LIBRARIES}` as it does not seem to work on some machines
target_link_libraries(${PROJECT_NAME} PRIVATE ${PORTAUDIO_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PORTAUDIO_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME} PUBLIC portaudio)

## ftgl
#find_package(ftgl REQUIRED)
#target_link_libraries(${PROJECT_NAME} PUBLIC ftgl)

# includes
target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_DIR})

# set C++ standard + compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# @optional set compiler flags for additional warnings or optimizations
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wno-c++17-extensions")
