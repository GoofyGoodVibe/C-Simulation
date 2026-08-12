#include <SDL3/SDL.h>
#include "assets.h"
#include "particle.h"
#include "general.h"
#include <stdlib.h>
#include <time.h>


int main(void)
{
    // Initialize SDL3
    SDLContext context = InitSDL(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!context.window) {
        SDL_Log("Failed to initialize SDL3. Exiting.");
        return 1;
    }

    SDL_Window *window = context.window;
    SDL_Renderer *renderer = context.renderer;
    
    StartUp(); // Initialize thread pool and particle threading

    // create a random seed for particle generation
    srand((unsigned int)time(NULL));

    // Create particle asset and texture
    Asset particle_asset = assets_load("particle", "assets/images/particle.bmp", renderer);

    if(!particle_asset.texture) {
        SDL_Log("Failed to load particle asset. Exiting.");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // initialize particle system
    ParticleSystem particle_system = particle_system_init();
    particles_setup(WINDOW_WIDTH, WINDOW_HEIGHT, &particle_system);


    // Main loop
    int running = 1;
    float delta_time = 1.0f / 60.0f; // Assuming 60 FPS
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Update Everything before Render
        Update(&particle_system, delta_time, WINDOW_WIDTH, WINDOW_HEIGHT);

        // Clear Renderer
        SDL_SetRenderDrawColor(renderer, 18, 18, 22, 255);
        SDL_RenderClear(renderer);

        // Draw Everything
        Draw(renderer, &particle_system, &particle_asset);
        SDL_RenderPresent(renderer);
    }

    // Destroy Assets and Textures
    assets_unload(&particle_asset);

    // Clean up and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
