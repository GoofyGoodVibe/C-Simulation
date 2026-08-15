#include <SDL3/SDL.h>

#include "update.h"
#include "init.h"
#include "general.h"



int main(void)
{
    // Initialize SDL3
    SDLContext context = InitSDL(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!context.window) {
        SDL_Log("Failed to initialize SDL3. Exiting.");
        return 1;
    }

    Run_Update_Loop(context);


    return 0;
}
