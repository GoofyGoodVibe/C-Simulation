#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>


#include "obstacle.h"
#include "boid.h"
#include "init.h"

void init_obstacle_manager(ObstacleManager *mgr) {
    mgr->count = 0;
    mgr->capacity = 10;
    mgr->obstacles = (Obstacle*)malloc(mgr->capacity * sizeof(Obstacle));
}

void add_obstacle(ObstacleManager *mgr, Vector2D pos, float radius) {
    if (mgr->count >= mgr->capacity) {
        mgr->capacity *= 2;
        mgr->obstacles = (Obstacle*)realloc(mgr->obstacles, mgr->capacity * sizeof(Obstacle));
    }
    mgr->obstacles[mgr->count].position = pos;
    mgr->obstacles[mgr->count].radius = radius;
    mgr->count++;
}

Vector2D calculate_obstacle_avoidance(Boid *boid, ObstacleManager *mgr) {
    Vector2D steering = {0.0f, 0.0f};
    int tracking_count = 0;

    for (int i = 0; i < mgr->count; i++) {
        Obstacle *obs = &mgr->obstacles[i];
        
        // Vector pointing from obstacle center to boid position
        Vector2D escape_dir = vec_sub(boid->position, obs->position);
        float distance = vec_mag(escape_dir);
        
        // Define a dynamic safety buffer zone around the physical obstacle radius
        float safety_zone = obs->radius + Boid_Obstacle_Buffer;

        if (distance < safety_zone && distance > 0.0f) {
            // Push away inversely proportional to proximity
            Vector2D push_vector = vec_mult(escape_dir, 1.0f / distance);
            steering = vec_add(steering, push_vector);
            tracking_count++;
        }
    }

    if (tracking_count > 0) {
        steering = vec_mult(steering, 1.0f / tracking_count);
        float mag = vec_mag(steering);
        if (mag > 0.0f) {
            steering = vec_mult(steering, 1.0f / mag);
            // Scale up to Max Speed away from the object
            steering = vec_mult(steering, Boid_Max_Speed);
        }
        
        // Steering = Desired - Current
        steering = vec_sub(steering, boid->velocity);
        
        // Give obstacle evasion high structural authority to break flock paths
        steering = vec_limit(steering, Boid_Max_Force * Boid_Obstacle_Weight);
    }

    return steering;
}

void free_obstacle_manager(ObstacleManager *mgr) {
    if (mgr->obstacles) {
        free(mgr->obstacles);
        mgr->obstacles = NULL;
    }
    mgr->count = 0;
    mgr->capacity = 0;
}




void draw_obstacle(SDLContext *context, Obstacle *obstacle, SDL_Texture *obstacle_texture) {
    if (!obstacle_texture) return;

    // Define a highly obvious obstacle color (e.g., Bright Crimson Red: 255, 30, 60)
    Uint8 r = 255, g = 30, b = 60;

    // 1. Tint the hardware texture using SDL3 color modulation formulas
    SDL_SetTextureColorMod(obstacle_texture, r, g, b);

    // 2. Set up the destination floating-point boundary box centered on the position coordinates
    float diameter = obstacle->radius * 2.0f;
    SDL_FRect dest_rect = {
        .x = obstacle->position.x - obstacle->radius,
        .y = obstacle->position.y - obstacle->radius,
        .w = diameter,
        .h = diameter
    };

    // 3. Render the texture precisely onto the screen context target frame at subpixel precision
    // (Assuming context->renderer points to your active SDL_Renderer)
    SDL_RenderTexture(context->renderer, obstacle_texture, NULL, &dest_rect);

    // 4. Structural Cleanliness: Reset the texture modulation back to standard baseline white
    SDL_SetTextureColorMod(obstacle_texture, 255, 255, 255);
}

void draw_all_obstacles(SDLContext *context, ObstacleManager *mgr, SDL_Texture *obstacle_texture) {
    for (int i = 0; i < mgr->count; i++) {
        draw_obstacle(context, &mgr->obstacles[i], obstacle_texture);
    }
}
