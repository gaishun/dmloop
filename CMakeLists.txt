cmake_minimum_required(VERSION 3.16.3)
project(dmloop C)

set(CMAKE_C_STANDARD 99)


option(_DEBUG   "if debug"  ON)
option(_EXEC    "if exec"   OFF)
if(_DEBUG)
    add_compile_options(-D_DEBUG)
    message("_DEBUG ON")
endif()

if(_EXEC)
    add_compile_options(-D_EXEC)
    message("_EXEC ON")
endif()

add_executable(dmloop main.c)
