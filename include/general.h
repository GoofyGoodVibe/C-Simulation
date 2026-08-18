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
#define Boid_Amount 1000

// 1. Balanced Behavior Multipliers
#define Boid_Separation_Weight 0.6f   // Higher priority to prevent crowding
#define Boid_Alignment_Weight  0.5f   // Good group flow matching
#define Boid_Cohesion_Weight   0.03f   // Keeps them pulling back toward the group

// 2. Realistic Physics Capabilities
#define Boid_Max_Speed         200.0f  // Pixels per second forward velocity
#define Boid_Min_Speed         80.0f   // Minimum speed to avoid stalling
#define Boid_Max_Force         150.0f  // Smooth turning power (keep this lower than Max_Speed!)
#define Boid_Mass              1.0f
#define Boid_Perception_Radius 120.0f  // Lowered slightly so they don't track boids across the entire map
#define Boid_Separation_Radius 70.0f   // Slightly wider comfort zone to avoid overlaps
#define Boid_Separation_Threshold 30.0f // Closet a boid can get before separation force is multiplied by a higher factor to push them away
#define Boid_Vision_Angle      140.0f

// Obstacle Core Tuning Parameters
#define Boid_Obstacle_Weight 2.5f    // High priority so they break flocking lines to save themselves
#define Boid_Obstacle_Buffer 40.0f   // Buffer zone (in pixels) outside the obstacle radius where boids start turning
#define Boid_Default_Obstacle_Radius 30.0f // Size of obstacle dropped via right click
#define Boid_Boundary_Force 12.0f

#define Boid_Mouse_Seek_Weight 1.2f  // Gentle pull so they don't lose their bird-like string structures
extern int g_IsLeftMouseDown;
extern Vector2D g_MousePosition;




float getRandomFloat(float min, float max);
// Quick helper to limit a vector's length
Vector2D vec_add(Vector2D v1, Vector2D v2);
Vector2D vec_sub(Vector2D v1, Vector2D v2);
Vector2D vec_mult(Vector2D v, float scalar);
Vector2D vec_limit(Vector2D v, float max_val);


float vec_mag(Vector2D v);
float vec_dot(Vector2D v1, Vector2D v2);
#endif // GENERAL_H