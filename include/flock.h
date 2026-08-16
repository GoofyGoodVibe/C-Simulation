#ifndef FLOCK_H
#define FLOCK_H

#include "boid.h"
#include "init.h"
#include "obstacle.h"

typedef struct Flock {
    Boid *boids;
    int count;
} Flock;

void initialize_flock(Flock *flock, int count);
void update_flock(Flock *flock, ObstacleManager *obstacle_mgr);
void draw_flock(SDLContext *context, Flock *flock);
Flock create_flock(int count);

#endif
