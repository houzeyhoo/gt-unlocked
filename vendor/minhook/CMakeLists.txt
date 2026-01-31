set(SOURCES
    src/buffer.c
    src/hook.c
    src/trampoline.c
    src/hde/hde64.c
)

if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    list(APPEND SOURCES src/hde/hde32.c)
else()
    list(APPEND SOURCES src/hde/hde64.c)
endif()

add_library(minhook STATIC ${SOURCES})
target_include_directories(minhook PUBLIC include)
