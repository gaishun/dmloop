cmake_minimum_required(VERSION 3.16.3)
project(dmloop C)

set(CMAKE_C_STANDARD 99)


option(_DEBUG "if debug" ON)

if(_DEBUG)
    add_compile_options(-D_DEBUG)
    message("_DEBUG ON")

endif()

add_executable(dmloop main.c)
