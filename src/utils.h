#pragma once
#include <stdint.h>

#include <windows.h>

#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

HWND get_game_window();
HMODULE get_game_exe_module();
void *patch_memory(void *dest, const void *src, size_t count);
void *find_memory(const uint16_t *pattern, size_t n);
