set(SOURCES 
    dinput8.c
    dinput8.def
    fps.c
    utils.c
)

add_library(dinput8 SHARED ${SOURCES})
target_link_libraries(dinput8 PRIVATE minhook)
target_compile_definitions(dinput8 PRIVATE -D_CRT_SECURE_NO_WARNINGS)
target_compile_options(dinput8 PRIVATE /W3)
