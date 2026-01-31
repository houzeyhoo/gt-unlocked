#include "fps.h"

#include <windows.h>

SetFPSLimit_t SetFPSLimit_real = NULL;

void set_optimal_fps_limit(void)
{
    DEVMODE dm = {.dmSize = sizeof(DEVMODE)};

    /*
     * The game window is always tied to the primary display, so iteration through display devices is unnecessary.
     * See https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumdisplaysettingsa
     */
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        SetFPSLimit_real(NULL, (float)dm.dmDisplayFrequency);
    }
    else
    {
        /* Fall back to 60 FPS */
        SetFPSLimit_real(NULL, 60.0f);
    }
}

void __fastcall SetFPSLimit_hook(void *arg, float fps)
{
    set_optimal_fps_limit();
}