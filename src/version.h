#pragma once
#include <stdint.h>

// Modern (V4.00+?) clients hook NtProtectVirtualMemory to prevent tampering. Comment out if compiling for an older
// version
#define FIX_NTPROTECTVIRTUALMEMORY 1

// NOLINTBEGIN
#ifdef _WIN64
// Standalone/legacy clients

static const uint8_t NtProtectVirtualMemoryPrologue[] = {0x4c, 0x8b, 0xd1, 0xb8, 0x50};
static const char *SetFPSLimitPattern =
    "4C 8B DC 48 81 EC C8 00 00 00 48 8B ? ? ? ? ? 48 33 C4 48 89 84 24 B0 00 00 00 0F 57 C0 0F 2F C8";

#else
// Steam/Ubiconnect clients

static const uint8_t NtProtectVirtualMemoryPrologue[] = {0xb8, 0x50, 0x00, 0x00, 0x00};
static const char *SetFPSLimitPattern = "55 8B EC 6A FF 68 ? ? ? ? 64 A1 00 00 00 00 50 83 EC 6C A1 ? ? ? ? 33 C5 89 "
                                        "45 F0 50 8D 45 F4 64 A3 00 00 00 00 F3 0F 10 4D 08";

#endif
// NOLINTEND
