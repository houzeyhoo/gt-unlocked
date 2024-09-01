#include "memory.h"
#include <psapi.h>

HMODULE get_game_module() { return GetModuleHandleA(NULL); }

HWND get_game_window() { return FindWindowA(NULL, "Growtopia"); }

void* memcpyp(void* dest, const void* src, size_t num)
{
    DWORD old = 0;
    if (!VirtualProtect(dest, num, PAGE_EXECUTE_READWRITE, &old))
        return NULL;
    void* out = memcpy(dest, src, num);
    /* Return NULL if unable to restore old page protection. */
    return VirtualProtect(dest, num, old, &old) ? out : NULL;
}

void* find_pattern(const uint32_t* pattern, size_t n)
{
    if (pattern == NULL || n == 0)
        return 0;
    MODULEINFO mi;
    if (!K32GetModuleInformation(GetCurrentProcess(), get_game_module(), &mi, sizeof(mi)))
        return 0;
    uint8_t* ptr = (uint8_t*)mi.lpBaseOfDll;
    size_t size = mi.SizeOfImage;
    /* Find first non-wildcard (-1) byte in pattern. If first == pattern size, then anything
     * matches. */
    size_t first = 0;
    while (first < n && pattern[first] == -1)
        ++first;
    if (first == n)
        return (void*)ptr;
    /* Perform actual search. */
    for (size_t i = 0; i <= size - n; ++i)
    {
        int ok = 1;
        for (size_t j = first; j < n; ++j)
        {
            if (pattern[j] == -1)
                continue;
            if (ptr[i + j] != (uint8_t)pattern[j])
            {
                ok = 0;
                break;
            }
        }
        if (ok)
            return (void*)(ptr + i);
    }
    return 0;
}