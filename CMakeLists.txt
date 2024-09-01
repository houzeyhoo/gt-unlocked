cmake_minimum_required(VERSION 3.12)
project(gt-unlocked LANGUAGES C)

# Export compile_commands.json for clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
# Make sure we're using MSVC.
if(NOT MSVC)
    message(FATAL_ERROR "Only MSVC is supported.")
endif()

# Set sources.
set (SOURCES 
    src/dinput8.def
    src/dinput8.c
    src/memory.c
    src/fps.c
)
set (SOURCES_MINHOOK 
    src/minhook/buffer.c
    src/minhook/hook.c
    src/minhook/trampoline.c
)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SOURCES_HDE src/minhook/hde/hde64.c)
else()
    set(SOURCES_HDE src/minhook/hde/hde32.c)
endif()

add_library(dinput8 SHARED ${SOURCES} ${SOURCES_MINHOOK} ${SOURCES_HDE})
target_compile_definitions(dinput8 PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_compile_options(dinput8 PRIVATE /W3)
