#include "assets.h"
#include <stdbool.h>
#include <SDL3/SDL.h>

Asset assets_load(const char *name, const char *path, SDL_Renderer *renderer) {
    Asset asset;
    asset.name = name;
    SDL_Surface *surface = SDL_LoadBMP(path);
    if(surface){
        const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(surface->format);
        Uint32 black_color = SDL_MapRGB(format, NULL, 0, 0, 0);
        SDL_SetSurfaceColorKey(surface, true, black_color);
    }
    asset.texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return asset;
}

void assets_unload(Asset *asset) {
    if (asset && asset->texture) {
        SDL_DestroyTexture(asset->texture);
        asset->texture = NULL;
    }
}


