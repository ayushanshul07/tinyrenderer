cmake_minimum_required(VERSION 3.15)

project(tinyrenderer)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/parser)


add_executable(tinyrenderer main.cpp)
target_compile_options(tinyrenderer PRIVATE -O3)
target_link_options(tinyrenderer PRIVATE -O3)
target_link_libraries(tinyrenderer PRIVATE tgaimage parser)