#include "fps.h"
#include "memory.h"
#include "minhook/minhook.h"
#include "version.h"
#include <stdio.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define EXPORT __declspec(dllexport)
#define BUFFER_SIZE (MAX_PATH + 32)

#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

void show_error_box(const char* msg) { MessageBoxA(NULL, msg, "Error", MB_ICONERROR | MB_OK); }

/* Hook SetFPSLimit. */
void setup()
{
    /* Reliable way to wait for the game to initialize (hooking stuff too early may crash). */
    SendMessageA(get_game_window(), WM_NULL, 0, 0);

#ifdef FIX_NTPROTECTVIRTUALMEMORY
    /* Need to un-hook NtProtectVirtualMemory. */
    HMODULE module = GetModuleHandleA("ntdll");
    FARPROC proc = GetProcAddress(module, "NtProtectVirtualMemory");
    if (!memcpyp((void*)proc, NtProtectVirtualMemory_prologue,
                 sizeof(NtProtectVirtualMemory_prologue)))
    {
        show_error_box("Failed to fix NtProtectVirtualMemory.");
        return;
    }
#endif

    void* addr = find_pattern(SetFPSLimit_signature, COUNT_OF(SetFPSLimit_signature));
    if (!addr)
    {
        show_error_box("Failed to find SetFPSLimit pattern. This might be because gt-unlocked is "
                       "out of date.");
        return;
    }

    if (MH_Initialize() != MH_OK)
    {
        show_error_box("Failed to initialize hooking.");
        return;
    }
    if (MH_CreateHook(addr, SetFPSLimit_hook, (void**)&SetFPSLimit_real) != MH_OK)
    {
        show_error_box("Failed to create SetFPSLimit hook.");
        return;
    }
    if (MH_EnableHook(addr) != MH_OK)
    {
        show_error_box("Failed to enable SetFPSLimit hook.");
        return;
    }

    SetFPSLimitOptimal(NULL);
    SetWindowTextA(get_game_window(), "Growtopia (FPS Unlocked)");
}

/* This is practically the "entry point". The game calls it once on initialization. */
EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE instance, DWORD version, REFIID id, LPVOID* out,
                                         LPVOID unk)
{
    /* Not sure if the game ever calls DirectInput8Create more than once, but better safe than
     * sorry. */
    static long initialized = 0;
    if (InterlockedCompareExchange(&initialized, 1, 0) == 0)
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)setup, NULL, 0, NULL);

    /* Load the real DirectInput8Create. Since the game depends on dinput8 anyway, we can assume
     * it's present on the system. */
    char path[BUFFER_SIZE] = {0};
    GetSystemDirectoryA(path, MAX_PATH);
    strcat(path, "\\dinput8.dll");
    HMODULE dinput8 = LoadLibraryA(path);
    typedef HRESULT(WINAPI * DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPVOID);
    DirectInput8Create_t real = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
    return real(instance, version, id, out, unk);
}