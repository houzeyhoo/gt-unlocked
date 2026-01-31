#pragma once

typedef void(__fastcall *SetFPSLimitFn)(void *, float);
extern SetFPSLimitFn originalSetFPSLimit;

void SetOptimalFPSLimit(void);
void __fastcall SetFPSLimit(void *this, float fps);
