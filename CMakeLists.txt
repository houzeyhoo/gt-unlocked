cmake_minimum_required(VERSION 3.25)
# Mirror client versioning
project(gt-unlocked LANGUAGES C VERSION 5.19)

# Make sure /MD is always used
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)

if(NOT MSVC)
    message(FATAL_ERROR "${PROJECT_NAME} must be built with MSVC")
endif()

add_subdirectory(vendor/minhook)

add_library(${PROJECT_NAME} SHARED
    src/dinput8.c
    src/dinput8.def
    src/fps.c
    src/utils.c
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "dinput8"
    SUFFIX ".dll"
)

# /W4 is too noisy
target_compile_options(${PROJECT_NAME} PRIVATE /W3)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minhook
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
