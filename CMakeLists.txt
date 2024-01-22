# SPDX-FileCopyrightText: 2023 Stephen Merrony
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.28)

include(pico_sdk_import.cmake)

project(midiplayer 
        VERSION 0.1.0
        LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

set(PICO_BOARD pico) # or pico_w
pico_sdk_init()

add_executable(
    midiplayer
    src/midiplayer.c
    src/midi.c
    src/servo.c
    src/speaker.c
    src/usb_descriptors.c
)

pico_add_extra_outputs(midiplayer)

target_compile_options(midiplayer PRIVATE -Werror -Wall)

target_include_directories(midiplayer PRIVATE ${CMAKE_CURRENT_LIST_DIR} src)

target_link_libraries(
    midiplayer
    hardware_clocks
    hardware_pwm
    pico_multicore
    pico_stdlib
    tinyusb_device
    tinyusb_board
    )
