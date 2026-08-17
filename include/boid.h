#ifndef BOID_H
#define BOID_H

#include "general.h"
#include "init.h"

// 1. Correct, type-safe forward declaration matching your typedef style
typedef struct Flock Flock; 
typedef struct ObstacleManager ObstacleManager;

typedef struct Boid{
    Vector2D position;
    Vector2D velocity;
    float size;
    float r, g, b, a;
} Boid;

Boid boid_create(float x, float y, float size, float r, float g, float b, float a);

// 2. Pass flock as a pointer (*flock) so it doesn't duplicate memory on the stack, 
// and add delta_time so your physics integration works.
void update_boid(Boid *boid, Flock *flock, ObstacleManager *obstacle_mgr);
void draw_boid(SDLContext *context, Boid *boid);
Vector2D calculate_Forces(Boid* boid, Flock *flock, ObstacleManager *obstacle_mgr);
Vector2D calculate_separation(Boid *boid, Flock *flock);
Vector2D calculate_alignment(Boid *boid, Flock *flock);
Vector2D calculate_cohesion(Boid *boid, Flock *flock);
Vector2D calculate_boundaries(Boid *boid);
Vector2D calculate_mouse_seek(Boid *boid, Vector2D mouse_pos);

#endif
