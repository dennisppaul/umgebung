cmake_minimum_required(VERSION 3.10)
project(umgebung-SDL3-transition)
find_package(SDL3 REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
