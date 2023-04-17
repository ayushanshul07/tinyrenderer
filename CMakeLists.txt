cmake_minimum_required(VERSION 3.15)

project(tinyrenderer)

add_subdirectory(external)

set(CMAKE_CXX_FLAGS "-O3")
add_executable(tinyrenderer main.cpp)

target_link_libraries(tinyrenderer PRIVATE tgaimage)