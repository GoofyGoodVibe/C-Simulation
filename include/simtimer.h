#ifndef SIM_TIMER_H
#define SIM_TIMER_H

#include <SDL3/SDL.h>

extern float g_DeltaTime;

/**
 * @brief Initializes the global timer subsystem.
 */
void Timer_Init(void);

/**
 * @brief Updates delta time. Call this exactly once at the absolute start of each frame loop.
 */
void Timer_Update(void);

/**
 * @brief Retrieves the current frame delta time in fractional seconds.
 * @note Safe to read from any thread simultaneously while the frame is processing.
 * @return Frame time in seconds (e.g., 0.016667 for 60 FPS).
 */
float Timer_GetDeltaTime(void);

/**
 * @brief Retrieves the current frames per second (FPS).
 * @return Calculated FPS as a float.
 */
float Timer_GetFPS(void);

/**
 * @brief Retrieves the current frame execution time in milliseconds.
 * @return Frame time in milliseconds (e.g., 16.67 ms for 60 FPS).
 */
float Timer_GetFrameTimeMs(void);

#endif // SIM_TIMER_H