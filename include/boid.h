#ifndef BOID_H
#define BOID_H


#include "general.h"
#include "init.h"

typedef struct {
    Vector2D position;
    Vector2D velocity;
    float size;
    float r, g, b, a;
} Boid;

Boid boid_create(float x, float y, float size, float r, float g, float b, float a);
void update_boid(Boid *boid, float delta_time);
void draw_boid(SDLContext *context, Boid *boid);



#endif