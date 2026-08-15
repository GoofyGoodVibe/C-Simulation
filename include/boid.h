#ifndef BOID_H
#define BOID_H

#include "general.h"
#include "init.h"

// 1. Correct, type-safe forward declaration matching your typedef style
typedef struct Flock Flock; 

typedef struct {
    Vector2D position;
    Vector2D velocity;
    float size;
    float r, g, b, a;
} Boid;

Boid boid_create(float x, float y, float size, float r, float g, float b, float a);

// 2. Pass flock as a pointer (*flock) so it doesn't duplicate memory on the stack, 
// and add delta_time so your physics integration works.
void update_boid(Boid *boid, Flock *flock);
void draw_boid(SDLContext *context, Boid *boid);

#endif
