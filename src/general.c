#include "general.h"
#include "particle.h"
#include "threadpool.h"
#include <SDL3/SDL.h>


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

    context.window = window;
    context.renderer = renderer;
    return context;
}

void StartUp() {
    pool_init(); // Initialize the thread pool    
    particles_threading_init();
}

void Update(ParticleSystem *particle_system, float delta_time, int window_width, int window_height) { // Throw all Update functions here
 
    particles_update(particle_system, delta_time, window_width, window_height);

}

void Draw(SDL_Renderer *renderer, ParticleSystem *particle_system, Asset *particle_asset) { // Throw all Draw functions here

    particles_draw(renderer, particle_system, particle_asset);

}