#include "fps.h"
#include <windows.h>

SetFPSLimitFn originalSetFPSLimit = NULL;

void SetOptimalFPSLimit(void)
{
    DEVMODE dm = {.dmSize = sizeof(DEVMODE)};

    // The client window is always created on the primary display device, so just check its refresh rate and set
    // the FPS limit accordingly
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        originalSetFPSLimit(NULL, (float)dm.dmDisplayFrequency);
    }
    else
    {
        // Fall back to 60 FPS
        originalSetFPSLimit(NULL, 60.0f);
    }
}

void __fastcall SetFPSLimit(void *this, float fps)
{
    SetOptimalFPSLimit();
}
