cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(test_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(02_BlockingUART
02_BlockingUART.c
)

target_link_libraries(02_BlockingUART pico_stdlib)
