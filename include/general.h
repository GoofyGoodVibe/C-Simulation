#ifndef GENERAL_H
#define GENERAL_H

#include <stdlib.h>
#include <math.h>

typedef struct {
    float x;
    float y;
} Vector2D;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define margin 25.0f


#define Boid_Seperation_Weight 0.5f
#define Boid_Alignment_Weight 0.4f
#define Boid_Cohesion_Weight 0.05f

#define Boid_Max_Speed 100.0f
#define Boid_Max_Force 160.0f
#define Boid_Perception_Radius 175.0f

#define Boid_Vision_Angle 270.0f

float getRandomFloat(float min, float max);
// Quick helper to limit a vector's length
void limitvector(Vector2D* vector, float max_length);

#endif // GENERAL_H