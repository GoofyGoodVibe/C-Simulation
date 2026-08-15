#ifndef FLOCK_H
#define FLOCK_H

#include "boid.h"
#include "init.h"
typedef struct {
    Boid *boids;
    int count;
} Flock;

void initialize_flock(Flock *flock, int count);
void update_flock(Flock *flock, float delta_time);
void draw_flock(SDLContext *context, Flock *flock);
Flock create_flock(int count);

#endif