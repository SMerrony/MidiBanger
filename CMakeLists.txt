# SPDX-FileCopyrightText: 2023 Stephen Merrony
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.28)

include(pico_sdk_import.cmake)

project(midibanger LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

set(PICO_BOARD pico)
# set(PICO_BOARD pico_w)
# set(PICO_BOARD waveshare_rp2040_zero)
pico_sdk_init()

add_executable(
    midibanger
    src/midibanger.c
    src/midi.c
    src/servo.c
    src/speaker.c
    src/usb_descriptors.c
)

pico_add_extra_outputs(midibanger)

target_compile_options(midibanger PRIVATE -Werror -Wall)

target_include_directories(midibanger PRIVATE ${CMAKE_CURRENT_LIST_DIR} src)

target_link_libraries(
    midibanger
    hardware_clocks
    hardware_pwm
    pico_multicore
    pico_stdlib
    tinyusb_device
    tinyusb_board
    )

pico_set_program_url(midibanger "https://github.com/SMerrony/MidiBanger") 
