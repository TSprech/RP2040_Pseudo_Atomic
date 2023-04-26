cmake_minimum_required(VERSION 3.24)
include(ARMGCCToolchain.cmake)

include(pico-sdk/pico_sdk_init.cmake)

project(RP2040_Pseudo_Atomic C CXX ASM)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_link_options(-Wl,-gc-sections,--print-memory-usage)

if (TARGET tinyusb_device)
    add_executable(RP2040_Pseudo_Atomic Src/main.cpp)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(RP2040_Pseudo_Atomic 1)
    pico_enable_stdio_uart(RP2040_Pseudo_Atomic 0)

    # pull in common dependencies
    target_link_libraries(RP2040_Pseudo_Atomic pico_stdlib pico_multicore)

    # pull in common dependencies and additional pwm hardware support
    target_link_libraries(RP2040_Pseudo_Atomic pico_stdlib hardware_pwm hardware_adc)

    # create map/bin/hex file etc.
    pico_add_extra_outputs(RP2040_Pseudo_Atomic)

    target_include_directories(RP2040_Pseudo_Atomic PUBLIC Inc)
    add_link_options(--print-memory-usage)
elseif (PICO_ON_DEVICE)
    message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif ()