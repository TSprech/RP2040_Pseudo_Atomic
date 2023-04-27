cmake_minimum_required(VERSION 3.24)
add_library(RP2040Atomic INTERFACE)

target_include_directories(RP2040Atomic INTERFACE ${CMAKE_CURRENT_LIST_DIR}/Inc)
target_link_libraries(RP2040Atomic INTERFACE pico_multicore)
