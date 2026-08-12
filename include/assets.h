#ifndef ASSETS_H
#define ASSETS_H

#include <SDL3/SDL.h>

typedef struct{
    SDL_Texture *texture;
    const char *name;
} Asset;

Asset assets_load(const char *name, const char *path, SDL_Renderer *renderer);
void assets_unload(Asset *asset);


#endif // ASSETS_H