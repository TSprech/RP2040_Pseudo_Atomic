cmake_minimum_required(VERSION 3.24)

set(CMAKE_SYSTEM_NAME Generic) # No OS
set(CMAKE_SYSTEM_VERSION 1)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
# Prevent CMake from attempting to compile the default test as it does not work for for embedded toolchains
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
