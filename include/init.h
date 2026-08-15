#ifndef INIT_H
#define INIT_H

#include <SDL3/SDL.h>

#include "assets.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Asset particle_asset; 
} SDLContext;

// Function prototypes for initialization
SDLContext InitSDL(int window_width, int window_height);
void cleanup_sdl(SDLContext context);

#endif // INIT_H