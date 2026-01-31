#include <minhook.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>

#include "fps.h"
#include "utils.h"
#include "version.h"

#include <stdio.h>

#define PATTERN_BUFFER_MAX_COUNT 1024

typedef HRESULT(WINAPI *DirectInput8CreateFn)(HINSTANCE, DWORD, REFIID, LPVOID *, LPVOID);
static DirectInput8CreateFn originalDirectInput8Create = NULL;

int FixNtProtectVirtualMemoryIfNeeded(void)
{
#ifdef FIX_NTPROTECTVIRTUALMEMORY
    HMODULE ntdll = GetModuleHandleA("ntdll");
    void *fn = GetProcAddress(ntdll, "NtProtectVirtualMemory");
    if (!PatchMemory(fn, NtProtectVirtualMemoryPrologue, sizeof(NtProtectVirtualMemoryPrologue)))
    {
        return 0;
    }
#endif
    return 1;
}

int HookSetFPSLimit(void)
{
    OptionalByte patternBytes[PATTERN_BUFFER_MAX_COUNT] = {0};
    int32_t patternLength = ParsePattern(SetFPSLimitPattern, patternBytes, PATTERN_BUFFER_MAX_COUNT);

    if (patternLength <= 0)
    {
        return 0;
    }
    void *fn = FindMemory(patternBytes, (size_t)patternLength);

    if (!fn)
    {
        return 0;
    }
    if (MH_CreateHook(fn, SetFPSLimit, (void **)&originalSetFPSLimit) != MH_OK)
    {
        return 0;
    }
    if (MH_EnableHook(fn) != MH_OK)
    {
        return 0;
    }

    return 1;
}

void Setup(void)
{
    SendMessageA(GetClientWindow(), WM_NULL, 0, 0);

    if (!FixNtProtectVirtualMemoryIfNeeded())
    {
        ShowErrorMessageBox("Failed to patch NtProtectVirtualMemory. Please report this issue on GitHub.");
        return;
    }

    if (MH_Initialize() != MH_OK)
    {
        ShowErrorMessageBox("Failed to initialize MinHook. Please report this issue on GitHub.");
        return;
    }

    if (!HookSetFPSLimit())
    {
        ShowErrorMessageBox("Failed to hook SetFPSLimit. Please report this issue on GitHub.");
        return;
    }

    SetOptimalFPSLimit();
    SetWindowTextA(GetClientWindow(), "Growtopia (FPS Unlocked)");
}

void LoadOriginalDirectInput8Create(void)
{
    // Create path to the system dinput8.dll
    uint32_t sysPathSize = GetSystemDirectoryW(NULL, 0);
    uint32_t modulePathSize = sysPathSize + (uint32_t)wcslen(L"\\dinput8.dll") + 1;
    wchar_t *modulePath = (wchar_t *)calloc(modulePathSize, sizeof(wchar_t));
    if (!modulePath)
    {
        return;
    }
    GetSystemDirectoryW(modulePath, sysPathSize);
    wcsncat(modulePath, L"\\dinput8.dll", modulePathSize - wcslen(modulePath) - 1);

    // Load original function
    HMODULE dinput8 = LoadLibraryW(modulePath);
    free(modulePath);
    if (!dinput8)
    {
        return;
    }
    originalDirectInput8Create = (DirectInput8CreateFn)GetProcAddress(dinput8, "DirectInput8Create");
}

HRESULT WINAPI DirectInput8Create(HINSTANCE hInst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPVOID punkOuter)
{
    static long isInitialized = 0;
    if (!InterlockedCompareExchange(&isInitialized, 1, 0))
    {
        LoadOriginalDirectInput8Create();
        if (originalDirectInput8Create)
        {
            // On older versions, we can run setup on the main thread, which is preferable, as we don't have to worry
            // about potential concurrency issues. It crashes on newer versions, I think because they added an
            // integrity check that runs after gamepad initialization. Running the setup on its own thread lets the
            // main thread go through the check before we do any modifications
            HANDLE setupThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Setup, NULL, 0, NULL);
            if (setupThread)
            {
                CloseHandle(setupThread);
            }
        }
    }

    if (!originalDirectInput8Create)
    {
        // LoadOriginalDirectInput8Create must have failed
        ShowErrorMessageBox("Failed to load original DirectInput8Create function. Please report this issue on GitHub.");
        return E_FAIL;
    }
    return originalDirectInput8Create(hInst, dwVersion, riidltf, ppvOut, punkOuter);
}

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hInstDLL);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
