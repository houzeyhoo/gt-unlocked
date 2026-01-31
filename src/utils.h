#pragma once
#include <stdint.h>
#include <wtypesbase.h>

#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

#define WILDCARD_BYTE ((uint16_t)-1)
typedef uint16_t OptionalByte;

HWND GetClientWindow();
HMODULE GetClientExeModule();

// Converts IDA-style patterns to raw byte arrays
int32_t ParsePattern(const char *pattern, OptionalByte *outBytes, size_t maxCount);

void *PatchMemory(void *address, const void *data, size_t size);
void *FindMemory(const uint16_t *pattern, size_t count);

void ShowErrorMessageBox(const char *message);
