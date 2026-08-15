#include <SDL3/SDL.h>
#include "init.h"
#include "boid.h"
#include "general.h"
#include "flock.h"
#include "simtimer.h"
#include <math.h>

Boid boid_create(float x, float y, float size, float r, float g, float b, float a) {
    Boid boid = {{x, y}, {getRandomFloat(-Boid_Max_Speed, Boid_Max_Speed), getRandomFloat(-Boid_Max_Speed, Boid_Max_Speed)}, size, r, g, b, a};
    return boid;
}

void update_boid(Boid *boid, Flock *flock) {
    Vector2D seperation_force = {0.0f, 0.0f}; 
    Vector2D alignment_force = {0.0f, 0.0f}; 
    Vector2D cohesion_center = {0.0f, 0.0f}; 
    int neighbor_count = 0; 

    float Boid_Separation_Radius = 25.0f; 

    // Calculate heading vector
    float current_speed_sqr = boid->velocity.x * boid->velocity.x + boid->velocity.y * boid->velocity.y;
    Vector2D heading = {1.0f, 0.0f};
    if (current_speed_sqr > 0.00001f) {
        float current_speed = sqrtf(current_speed_sqr);
        heading.x = boid->velocity.x / current_speed;
        heading.y = boid->velocity.y / current_speed;
    }
    float vision_cos_threshold = cosf((Boid_Vision_Angle * 0.5f) * (3.14159265f / 180.0f));

    for(int i = 0; i < flock->count; i++) {
        Boid *other = &flock->boids[i];
        if (other != boid) {
            
            Vector2D distance_vector = {other->position.x - boid->position.x, other->position.y - boid->position.y};
            Vector2D distance_sqr = {distance_vector.x * distance_vector.x, distance_vector.y * distance_vector.y};
            float distance_sqrd = distance_sqr.x + distance_sqr.y;

            if (distance_sqrd < Boid_Perception_Radius * Boid_Perception_Radius) {
                
                int is_visible = 0;
                if (distance_sqrd > 0.00001f) {
                    float dist = sqrtf(distance_sqrd);
                    Vector2D to_other = {distance_vector.x / dist, distance_vector.y / dist};
                    float dot_product = heading.x * to_other.x + heading.y * to_other.y;
                    if (dot_product >= vision_cos_threshold) is_visible = 1;
                } else {
                    is_visible = 1;
                }

                if (is_visible) {
                    // Separation accumulation
                    if (distance_sqrd < Boid_Separation_Radius * Boid_Separation_Radius && distance_sqrd > 0) { 
                        seperation_force.x -= distance_vector.x / distance_sqrd;
                        seperation_force.y -= distance_vector.y / distance_sqrd;
                    }

                    // Alignment accumulation
                    alignment_force.x += other->velocity.x;
                    alignment_force.y += other->velocity.y;

                    // Cohesion accumulation
                    cohesion_center.x += other->position.x;
                    cohesion_center.y += other->position.y;

                    neighbor_count++; 
                }
            }
        }
    }

    Vector2D total_steering = {0.0f, 0.0f};

    if (neighbor_count > 0) {
        // --- 1. PROPER ALIGNMENT STEERING ---
        alignment_force.x /= neighbor_count;
        alignment_force.y /= neighbor_count;
        float align_mag = sqrtf(alignment_force.x * alignment_force.x + alignment_force.y * alignment_force.y);
        Vector2D alignment_force_final = {0.0f, 0.0f};
        if (align_mag > 0.0001f) {
            alignment_force_final.x = ((alignment_force.x / align_mag) * Boid_Max_Speed) - boid->velocity.x;
            alignment_force_final.y = ((alignment_force.y / align_mag) * Boid_Max_Speed) - boid->velocity.y;
        }

        // --- 2. PROPER COHESION STEERING ---
        cohesion_center.x /= neighbor_count;
        cohesion_center.y /= neighbor_count;
        Vector2D to_center = {cohesion_center.x - boid->position.x, cohesion_center.y - boid->position.y};
        float dist_to_center = sqrtf(to_center.x * to_center.x + to_center.y * to_center.y);
        Vector2D cohesion_force_final = {0.0f, 0.0f};
        if (dist_to_center > 0.0001f) {
            cohesion_force_final.x = ((to_center.x / dist_to_center) * Boid_Max_Speed) - boid->velocity.x;
            cohesion_force_final.y = ((to_center.y / dist_to_center) * Boid_Max_Speed) - boid->velocity.y;
        }

        // --- 3. PROPER SEPARATION STEERING ---
        float sep_mag = sqrtf(seperation_force.x * seperation_force.x + seperation_force.y * seperation_force.y);
        Vector2D seperation_force_final = {0.0f, 0.0f};
        if (sep_mag > 0.0001f) {
            seperation_force_final.x = ((seperation_force.x / sep_mag) * Boid_Max_Speed) - boid->velocity.x;
            seperation_force_final.y = ((seperation_force.y / sep_mag) * Boid_Max_Speed) - boid->velocity.y;
        }

        // Combine weighted flocking steering vectors
        total_steering.x = (seperation_force_final.x * Boid_Seperation_Weight) + (alignment_force_final.x * Boid_Alignment_Weight) + (cohesion_force_final.x * Boid_Cohesion_Weight);
        total_steering.y = (seperation_force_final.y * Boid_Seperation_Weight) + (alignment_force_final.y * Boid_Alignment_Weight) + (cohesion_force_final.y * Boid_Cohesion_Weight);
    }

    // --- 4. SMOOTH BOUNDARY AVOIDANCE STEERING ---
    Vector2D boundary_desired = {boid->velocity.x, boid->velocity.y};
    int near_boundary = 0;
    Vector2D center = {WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f};
    

     // Hard Check: If completely off-screen, force-steer directly back to the center
    if (boid->position.x < 0 || boid->position.x > WINDOW_WIDTH || 
        boid->position.y < 0 || boid->position.y > WINDOW_HEIGHT) {
        
        Vector2D to_center = {center.x - boid->position.x, center.y - boid->position.y};
        float dist = sqrtf(to_center.x * to_center.x + to_center.y * to_center.y);
        if (dist > 0.001f) {
            boundary_desired.x = (to_center.x / dist) * Boid_Max_Speed;
            boundary_desired.y = (to_center.y / dist) * Boid_Max_Speed;
            near_boundary = 1;
        }
    } 
    // Soft Check: Standard behavior if they are within the margin
    else {
        if (boid->position.x < margin) {
            boundary_desired.x = Boid_Max_Speed;
            near_boundary = 1;
        } else if (boid->position.x > WINDOW_WIDTH - margin) {
            boundary_desired.x = -Boid_Max_Speed;
            near_boundary = 1;
        }
        if (boid->position.y < margin) {
            boundary_desired.y = Boid_Max_Speed;
            near_boundary = 1;
        } else if (boid->position.y > WINDOW_HEIGHT - margin) {
            boundary_desired.y = -Boid_Max_Speed;
            near_boundary = 1;
        }
    }

    // --- 5. CRITICAL BUG FIX: INDEPENDENT INTEGRATION AND CLAMP ---
    
    // First, safely clamp normal flocking behavior so boids don't twitch out inside their schools
    limitvector(&total_steering, Boid_Max_Force);

    // Second, if they are near a wall, apply an unthrottled panic steering force override
    if (near_boundary) {
        Vector2D boundary_steering = {boundary_desired.x - boid->velocity.x, boundary_desired.y - boid->velocity.y};
        
        // Give wall reactions an explicit high-limit cap (e.g. 10x higher than normal max force)
        // This gives them emergency brakes without making open water flocking jerky!
        float emergency_max_force = Boid_Max_Force * 10.0f;
        limitvector(&boundary_steering, emergency_max_force);

        total_steering.x += boundary_steering.x; 
        total_steering.y += boundary_steering.y;
    }

    // Apply acceleration force directly to velocity
    boid->velocity.x += total_steering.x * g_DeltaTime;
    boid->velocity.y += total_steering.y * g_DeltaTime;

    // Enforce top speed absolute limit
    limitvector(&boid->velocity, Boid_Max_Speed);

    // Physical position step integration
    boid->position.x += boid->velocity.x * g_DeltaTime;
    boid->position.y += boid->velocity.y * g_DeltaTime;
}

void draw_boid(SDLContext *context, Boid *boid) {
    SDL_SetTextureColorMod(context->particle_asset.texture, boid->r, boid->g, boid->b);
    SDL_SetTextureAlphaMod(context->particle_asset.texture, boid->a);
    SDL_FRect dst = { boid->position.x - boid->size * 0.5f, boid->position.y - boid->size * 0.5f, boid->size, boid->size };
    SDL_RenderTexture(context->renderer, context->particle_asset.texture, NULL, &dst);
    
    SDL_SetTextureColorMod(context->particle_asset.texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(context->particle_asset.texture, 255);
}
