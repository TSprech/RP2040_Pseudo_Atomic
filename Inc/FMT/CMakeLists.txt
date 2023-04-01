cmake_minimum_required(VERSION 3.23)

add_compile_definitions(FMT_STATIC_THOUSANDS_SEPARATOR=1)

add_compile_definitions(FMT_USE_FLOAT=1)
add_compile_definitions(FMT_USE_DOUBLE=0)
add_compile_definitions(FMT_USE_LONG_DOUBLE=0)

add_subdirectory(fmt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})