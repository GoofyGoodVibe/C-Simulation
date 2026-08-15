#include <SDL3/SDL.h>

#include "update.h"
#include "init.h"
#include "simtimer.h"
#include "flock.h"

void Run_Update_Loop(SDLContext context) {
    SDL_Event event;
    int running = 1;
    Flock flock = create_flock(100); // Create a flock with 100 boids
    
    while (running) {
        
        Timer_Update();
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Update logic here
        // ...
        update_flock(&flock, g_DeltaTime);

        // Clear Renderer
        SDL_SetRenderDrawColor(context.renderer, 18, 18, 22, 255);
        SDL_RenderClear(context.renderer);

        // Draw Everything
        
        draw_flock(&context, &flock);

        SDL_RenderPresent(context.renderer);

    }

}