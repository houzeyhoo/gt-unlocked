#pragma once

typedef void(__fastcall* SetFPSLimit_t)(void*, float);
extern SetFPSLimit_t SetFPSLimit_real;

/* Sets optimal FPS limit for user display. */
void SetFPSLimitOptimal(void* rcx);

/* Sets Growtopia's framerate. */
void __fastcall SetFPSLimit_hook(void* rcx, float fps);
