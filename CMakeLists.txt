cmake_minimum_required(VERSION 3.18)
project(gt-unlocked C)

if(NOT MSVC)
    message(FATAL_ERROR "This project may only be built via MSVC.")
endif()

add_subdirectory(vendor/minhook)
add_subdirectory(src)
