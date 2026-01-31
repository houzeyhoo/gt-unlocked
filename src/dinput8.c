#include "fps.h"
#include "utils.h"
#include "version.h"

#include <minhook.h>
#include <stdlib.h>
#include <windows.h>

void show_error_box(const char *msg)
{
    MessageBoxA(NULL, msg, "Error", MB_ICONERROR | MB_OK);
}

void setup()
{
    /* Wait for the game window to be ready */
    SendMessageA(get_game_window(), WM_NULL, 0, 0);

#ifdef FIX_NTPROTECTVIRTUALMEMORY
    HMODULE ntdll_module = GetModuleHandleA("ntdll");
    FARPROC ntdll_proc = GetProcAddress(ntdll_module, "NtProtectVirtualMemory");
    if (!patch_memory((void *)ntdll_proc, NtProtectVirtualMemory_prologue, sizeof(NtProtectVirtualMemory_prologue)))
    {
        show_error_box("Failed to patch NtProtectVirtualMemory. If you're running an older client, you might have to "
                       "compile gt-unlocked yourself without NtProtectVirtualMemory patching enabled. If you're on a "
                       "modern client, please report this issue.");
        return;
    }
#endif

    void *address = find_memory(SetFPSLimit_signature, COUNT_OF(SetFPSLimit_signature));
    if (!address)
    {
        show_error_box(
            "Failed to find the SetFPSLimit memory pattern. This most likely means gt-unlocked is out-of-date and "
            "needs to be updated. If no newer releases are available, please report this issue.");
        return;
    }

    if (MH_Initialize() != MH_OK)
    {
        show_error_box("Failed to initialize MinHook. Please report this issue.");
        return;
    }
    if (MH_CreateHook(address, SetFPSLimit_hook, (void **)&SetFPSLimit_real) != MH_OK)
    {
        show_error_box("Failed to create the SetFPSLimit hook. Please report this issue.");
        return;
    }
    if (MH_EnableHook(address) != MH_OK)
    {
        show_error_box("Failed to enable the SetFPSLimit hook. Please report this issue.");
        return;
    }

    set_optimal_fps_limit();
    SetWindowTextA(get_game_window(), "Growtopia (FPS Unlocked)");
}

HRESULT __stdcall DirectInput8Create(HINSTANCE instance, DWORD version, REFIID id, LPVOID *out, LPVOID unk)
{
    /* Ensure initialization only occurs once */
    static long initialized = 0;
    if (InterlockedCompareExchange(&initialized, 1, 0) == 0)
    {
        HANDLE setup_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)setup, NULL, 0, NULL);
        if (setup_thread)
        {
            CloseHandle(setup_thread);
        }
    }

    /* See https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemdirectoryw */
    uint32_t sys32_path_len = GetSystemDirectoryW(NULL, 0);
    wchar_t *path = malloc(sys32_path_len * sizeof(wchar_t) + sizeof(L"\\dinput8.dll"));
    if (!path)
    {
        return E_FAIL;
    }
    GetSystemDirectoryW(path, sys32_path_len);

    wcscat(path, L"\\dinput8.dll");
    HMODULE dinput8 = LoadLibraryW(path);
    free(path);
    typedef HRESULT(WINAPI * DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID *, LPVOID);
    DirectInput8Create_t real = (DirectInput8Create_t)GetProcAddress(dinput8, "DirectInput8Create");
    return real ? real(instance, version, id, out, unk) : E_FAIL;
}