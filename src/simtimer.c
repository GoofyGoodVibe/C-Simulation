#include "simtimer.h"

// Define and allocate the actual memory for the global variable
float g_DeltaTime = 0.0f;

// Hidden static hardware tracking variables
static Uint64 g_frequency = 0;
static Uint64 g_last_time = 0;

void Timer_Init(void) {
    g_frequency = SDL_GetPerformanceFrequency();
    g_last_time = SDL_GetPerformanceCounter();
    g_DeltaTime = 0.0f;
}

void Timer_Update(void) {
    Uint64 current_time = SDL_GetPerformanceCounter();
    Uint64 frame_ticks = current_time - g_last_time;
    g_last_time = current_time;

    // Convert raw hardware clock ticks into fractional seconds
    float dt = (float)frame_ticks / (float)g_frequency;

    // Hard ceiling cap to prevent physics explosions during freezes or stalls
    if (dt > 0.1f) {
        dt = 0.1f;
    }
    
    // Guard against zero division if a frame registers instantly
    if (dt < 0.000001f) {
        dt = 0.000001f;
    }

    // Write to the global variable once per frame
    g_DeltaTime = dt;
}

float Timer_GetFPS(void) {
    return 1.0f / g_DeltaTime;
}

float Timer_GetFrameTimeMs(void) {
    return g_DeltaTime * 1000.0f;
}