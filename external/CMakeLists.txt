project(tgaimage)

add_library(tgaimage STATIC src/tgaimage.cpp)
target_include_directories(tgaimage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)