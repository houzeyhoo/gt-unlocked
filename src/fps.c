#include "fps.h"
#include <windows.h>

SetFPSLimit_t SetFPSLimit_real = NULL;

void SetFPSLimitOptimal(void* rcx)
{
    DEVMODE dm = {0};
    dm.dmSize = sizeof(DEVMODE);
    /* The Growtopia window will always be tied to the primary display, therefore I'm pretty sure
     * there's no need to iterate through monitors & display devices. Calling EnumDisplaySettingsA
     * and passing NULL as the device name should return the settings of the display tied to the
     * calling thread, which in this case should be the primary display. If unable to retrieve the
     * display settings, default to a safe 60 FPS.
     */
    if (EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &dm))
        SetFPSLimit_real(NULL, (float)dm.dmDisplayFrequency);
    else
        SetFPSLimit_real(NULL, 60.0f);
}

void __fastcall SetFPSLimit_hook(void* rcx, float fps)
{
    /* The rcx param doesn't actually seem to matter (?), but can't hurt to pass it along. */
    SetFPSLimitOptimal(rcx);
}