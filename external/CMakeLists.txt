project(external)

add_library(tgaimage STATIC src/tgaimage.cpp)
target_compile_options(tgaimage PRIVATE -O3)

target_include_directories(tgaimage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(geometry INTERFACE)
target_include_directories(geometry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)