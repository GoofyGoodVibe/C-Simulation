#include <SDL3/SDL.h>

#include "update.h"
#include "init.h"
#include "general.h"

int g_IsLeftMouseDown = 0;
Vector2D g_MousePosition = {0.0f, 0.0f};



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
