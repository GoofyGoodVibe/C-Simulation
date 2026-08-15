#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "init.h"
#include "simtimer.h"


SDLContext InitSDL(int window_width, int window_height) {
    SDLContext context;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        context.window = NULL;
        context.renderer = NULL;
        return context;
    }

    SDL_Window *window = SDL_CreateWindow("2D Simulation", window_width, window_height, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        context.window = NULL;
        context.renderer = NULL;
        return context;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        context.window = NULL;
        context.renderer = NULL;
        return context;
    }

        // Create particle asset and texture
    context.particle_asset = assets_load("particle", "assets/images/particle.bmp", renderer);

    if(!context.particle_asset.texture) {
        SDL_Log("Failed to load particle asset. Exiting.");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }


    context.window = window;
    context.renderer = renderer;

    // create a random seed for particle generation
    srand((unsigned int)time(NULL));

    Timer_Init();

    return context;
}


void cleanup_sdl(SDLContext context) {
    // Destroy Assets and Textures
    assets_unload(&context.particle_asset);

    // Clean up and exit
    SDL_DestroyRenderer(context.renderer);
    SDL_DestroyWindow(context.window);
    SDL_Quit();
}