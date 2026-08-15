#include <SDL3/SDL.h>

#include "flock.h"

void initialize_flock(Flock *flock, int count) {
    flock->count = count;
    flock->boids = (Boid *)malloc(count * sizeof(Boid));
    for (int i = 0; i < count; i++) {
            flock->boids[i] = boid_create(getRandomFloat(0, 1280), getRandomFloat(0, 720), 5.0f, getRandomFloat(0, 255), getRandomFloat(0, 255), getRandomFloat(0, 255), 255.0f);
        }
}

void update_flock(Flock *flock) {
    for (int i = 0; i < flock->count; i++) {
        update_boid(&flock->boids[i], flock);
    }
}

void draw_flock(SDLContext *context, Flock *flock) {
    for (int i = 0; i < flock->count; i++) {
        draw_boid(context, &flock->boids[i]);
    }
}

Flock create_flock(int count) {
    Flock flock;
    initialize_flock(&flock, count);
    return flock;
}