#include <SDL3/SDL.h>

#include "update.h"
#include "init.h"
#include "simtimer.h"
#include "flock.h"
#include "general.h"
#include "obstacle.h"

ObstacleManager g_ObstacleMgr;

void Run_Update_Loop(SDLContext context) {
    SDL_Event event;
    int running = 1;

    init_obstacle_manager(&g_ObstacleMgr); // Initialize the obstacle manager
    Flock flock = create_flock(Boid_Amount); // Create a flock with Boid_Amount boids
    
    while (running) {
        
        Timer_Update();
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    Vector2D mouse_pos = {event.button.x, event.button.y};
                    add_obstacle(&g_ObstacleMgr, mouse_pos, Boid_Default_Obstacle_Radius);
                }
            }
        }

        Vector2D mouse_pos;
        uint32_t mouse_state = SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
        if(mouse_state & SDL_BUTTON_LEFT) {
            g_IsLeftMouseDown = 1;
            g_MousePosition = mouse_pos;
        } else {
            g_IsLeftMouseDown = 0;
        }
        // Update logic here
        // ...
        update_flock(&flock, &g_ObstacleMgr); // Pass the obstacle manager to the flock update function

        // Clear Renderer
        SDL_SetRenderDrawColor(context.renderer, 18, 18, 22, 255);
        SDL_RenderClear(context.renderer);

        // Draw Everything
        draw_all_obstacles(&context, &g_ObstacleMgr, context.particle_asset.texture);

        draw_flock(&context, &flock);

        SDL_RenderPresent(context.renderer);

    }

}