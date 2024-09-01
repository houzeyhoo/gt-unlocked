#pragma once
#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Returns a handle to the Growtopia exe module. */
HMODULE get_game_module();

/* Returns a handle to Growtopia's main window. */
HWND get_game_window();

/* Behaves exactly like standard memcpy, with the exception that it lifts any existing memory page
 * protections. Returns dest ptr on success, null on failure. */
void* memcpyp(void* dest, const void* src, size_t num);

/* Finds a pattern of length n in Growtopia's exe module memory. Returns address of match on
 * success, null on failure. */
void* find_pattern(const uint32_t* pattern, size_t n);
