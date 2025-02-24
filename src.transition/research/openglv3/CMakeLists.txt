cmake_minimum_required(VERSION 3.10)
project(SDL3_OpenGL3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
find_package(SDL3 REQUIRED)
pkg_search_module(GLEW REQUIRED glew)
find_package(OpenGL REQUIRED)
pkg_search_module(GLM REQUIRED glm)

message(STATUS "SDL3 INCLUDE  : ${SDL3_INCLUDE_DIRS}")
message(STATUS "GLEW INCLUDE  : ${GLEW_INCLUDE_DIRS}")
message(STATUS "OPENGL INCLUDE: ${OPENGL_INCLUDE_DIRS}")
message(STATUS "GLM INCLUDE   : ${GLM_INCLUDE_DIRS}")

# Add executable
add_executable(${PROJECT_NAME}
        main.cpp
        PGraphicsOpenGLv33.cpp
        PImage.cpp
        PVector.cpp
        PMatrix2D.cpp
        PMatrix3D.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLEW_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLM_INCLUDE_DIRS})

#add_library(imgui STATIC
#        imgui/imgui.cpp
#        imgui/imgui_demo.cpp
#        imgui/imgui_draw.cpp
#        imgui/imgui_tables.cpp
#        imgui/imgui_widgets.cpp
#        imgui/backends/imgui_impl_sdl3.cpp
#        imgui/backends/imgui_impl_opengl3.cpp
#)
#target_include_directories(imgui PUBLIC imgui imgui/backends)

# Link libraries
target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} imgui)
