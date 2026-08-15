#include <SDL3/SDL.h>
#include "init.h"

#include "boid.h"
#include "general.h"

Boid boid_create(float x, float y, float size, float r, float g, float b, float a) {
    Boid boid = {{x, y}, {getRandomFloat(-1.0f, 1.0f), getRandomFloat(-1.0f, 1.0f)}, size, r, g, b, a};
    return boid;
}

void update_boid(Boid *boid, float delta_time) {
    boid->position.x += boid->velocity.x * delta_time;
    boid->position.y += boid->velocity.y * delta_time;
}

void draw_boid(SDLContext *context, Boid *boid) {
        SDL_SetTextureColorMod(context->particle_asset.texture, boid->r, boid->g, boid->b);
        SDL_SetTextureAlphaMod(context->particle_asset.texture, boid->a);
        SDL_FRect dst = { boid->position.x - boid->size * 0.5f, boid->position.y - boid->size * 0.5f, boid->size, boid->size };
        SDL_RenderTexture(context->renderer, context->particle_asset.texture, NULL, &dst);
        
        //reset the texture color and alpha to default after rendering the particle
        SDL_SetTextureColorMod(context->particle_asset.texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(context->particle_asset.texture, 255);
}