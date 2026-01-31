#include "utils.h"
#include <psapi.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <stdio.h>

HWND GetClientWindow()
{
    return FindWindowA(NULL, "Growtopia");
}

HMODULE GetClientExeModule()
{
    return GetModuleHandleA(NULL);
}

int32_t ParsePattern(const char *pattern, OptionalByte *outBytes, size_t maxCount)
{
    int32_t count = 0;
    const char *p = pattern;

    while (*p != '\0')
    {
        if (isspace((unsigned char)*p))
        {
            p++;
            continue;
        }

        if ((size_t)count >= maxCount)
        {
            // Provided buffer is too small
            return -1;
        }

        if (*p == '?')
        {
            outBytes[count++] = WILDCARD_BYTE;
            // Wildcard can be "?" or "??"
            p++;
            if (*p == '?')
            {
                p++;
            }
        }
        else if (isxdigit((unsigned char)*p))
        {
            char *end;
            OptionalByte value = (OptionalByte)strtoul(p, &end, 16);
            if (value > 0xFF)
            {
                // Invalid byte
                return -1;
            }
            outBytes[count++] = value;

            if (p == end)
            {
                // Parsing failed, skip ahead
                p++;
            }
            else
            {
                // All ok, move past number
                p = end;
            }
        }
        else
        {
            // Unexpected character
            return -1;
        }
    }
    return count;
}

void *PatchMemory(void *dest, const void *src, size_t size)
{
    DWORD oldProtection = 0;
    if (!VirtualProtect(dest, size, PAGE_EXECUTE_READWRITE, &oldProtection))
    {
        return NULL;
    }
    memcpy(dest, src, size);

    if (!VirtualProtect(dest, size, oldProtection, &oldProtection))
    {
        // Even though the copy operation finished, return NULL to indicate failure as failing to restore protection
        // will crash the client on some versions
        return NULL;
    }
    return (void *)dest;
}

void *FindMemory(const OptionalByte *pattern, size_t count)
{
    if (pattern == NULL || count == 0)
    {
        return NULL;
    }

    MODULEINFO moduleInfo = {0};
    if (!GetModuleInformation(GetCurrentProcess(), GetClientExeModule(), &moduleInfo, sizeof(moduleInfo)))
    {
        return NULL;
    }

    if (count > moduleInfo.SizeOfImage)
    {
        return NULL;
    }

    uint8_t *begin = moduleInfo.lpBaseOfDll;
    uint8_t *end = begin + moduleInfo.SizeOfImage;
    uint8_t *softEnd = end - count;

    // Locate first non-wildcard byte
    size_t anchorIndex = 0;
    while (anchorIndex < count && pattern[anchorIndex] == WILDCARD_BYTE)
    {
        ++anchorIndex;
    }

    if (anchorIndex == count)
    {
        // Entire pattern is wildcards, anything matches
        return begin;
    }

    uint8_t *p = begin;
    while (p <= softEnd)
    {
        // Find next occurrence of anchor byte
        size_t remaining = (size_t)(end - (p + anchorIndex));
        uint8_t *anchor = memchr(p + anchorIndex, pattern[anchorIndex], remaining);
        if (!anchor)
        {
            break;
        }

        uint8_t *candidate = anchor - anchorIndex;
        if (candidate > softEnd)
        {
            break;
        }

        // Proper full comparison
        int foundMatch = 1;
        for (size_t i = 0; i < count; i++)
        {
            if (pattern[i] != WILDCARD_BYTE && candidate[i] != pattern[i])
            {
                foundMatch = 0;
                break;
            }
        }

        if (foundMatch)
        {
            return candidate;
        }
        p = candidate + 1;
    }
    return NULL;
}

void ShowErrorMessageBox(const char *message)
{
    MessageBoxA(NULL, message, "Error", MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
}
