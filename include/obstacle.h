#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "general.h"
#include "init.h" // --- FIXED: Directly include init.h now ---

// Forward declarations for other types
typedef struct Boid Boid;

typedef struct {
    Vector2D position;
    float radius;
} Obstacle;

typedef struct ObstacleManager {
    Obstacle *obstacles;
    int count;
    int capacity;
} ObstacleManager;

// Core functions
void init_obstacle_manager(ObstacleManager *mgr);
void add_obstacle(ObstacleManager *mgr, Vector2D pos, float radius);
Vector2D calculate_obstacle_avoidance(Boid *boid, ObstacleManager *mgr);
void free_obstacle_manager(ObstacleManager *mgr);

// FIXED: Clean, rock-solid compilation signatures using explicit SDL types
void draw_obstacle(SDLContext *context, Obstacle *obstacle, SDL_Texture *obstacle_texture);
void draw_all_obstacles(SDLContext *context, ObstacleManager *mgr, SDL_Texture *obstacle_texture);

#endif // OBSTACLE_H
