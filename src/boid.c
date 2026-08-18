#include <SDL3/SDL.h>

#define _USE_MATH_DEFINES  // <-- Add this first!
#include <math.h>

#include "general.h"
#include "init.h"
#include "boid.h"
#include "flock.h"
#include "simtimer.h"
#include "obstacle.h"

Boid boid_create(float x, float y, float size, float r, float g, float b, float a) {
    Boid boid = {{x, y}, {getRandomFloat(-Boid_Max_Speed, Boid_Max_Speed), getRandomFloat(-Boid_Max_Speed, Boid_Max_Speed)}, size, r, g, b, a};
    return boid;
}

void update_boid(Boid *boid, Flock *flock, ObstacleManager *obstacle_mgr) {
    /*
    Vector2D seperation_force = {0.0f, 0.0f}; 
    Vector2D alignment_force = {0.0f, 0.0f}; 
    Vector2D cohesion_force = {0.0f, 0.0f}; 
    Vector2D boundary_force = {0.0f, 0.0f}; 
    Vector2D mouse_force = {0.0f, 0.0f}; 
    Vector2D obstacle_force = {0.0f, 0.0f}; 

    seperation_force = calculate_separation(boid, flock);
    alignment_force = calculate_alignment(boid, flock);
    cohesion_force = calculate_cohesion(boid, flock);
    boundary_force = calculate_boundaries(boid);
    
    // FIXED: Removed the ampersand '&' since obstacle_mgr is already a pointer
    obstacle_force = calculate_obstacle_avoidance(boid, obstacle_mgr); 

    if (g_IsLeftMouseDown) {
        mouse_force = calculate_mouse_seek(boid, g_MousePosition);
    }

    Vector2D total_force = {0.0f, 0.0f};
    total_force = vec_add(total_force, vec_mult(seperation_force, Boid_Separation_Weight));
    total_force = vec_add(total_force, vec_mult(alignment_force, Boid_Alignment_Weight));
    total_force = vec_add(total_force, vec_mult(cohesion_force, Boid_Cohesion_Weight));
    total_force = vec_add(total_force, boundary_force);
    total_force = vec_add(total_force, vec_mult(mouse_force, Boid_Mouse_Seek_Weight));
    
    // FIXED: Added obstacle avoidance to your total physical force accumulation!
    total_force = vec_add(total_force, obstacle_force); 

    // Newton's Second Law: Acceleration = Force / Mass
    Vector2D acceleration = vec_mult(total_force, 1.0f / Boid_Mass); 
    
    boid->velocity = vec_add(boid->velocity, vec_mult(acceleration, g_DeltaTime));
    
    // Enforce limits and cruise speeds
    #define Boid_Min_Speed 80.0f 
    float current_speed = vec_mag(boid->velocity);
    if (current_speed > Boid_Max_Speed) {
        boid->velocity = vec_limit(boid->velocity, Boid_Max_Speed);
    } else if (current_speed < Boid_Min_Speed) {
        if (current_speed > 0.01f) {
            boid->velocity = vec_mult(boid->velocity, Boid_Min_Speed / current_speed);
        } else {
            boid->velocity = (Vector2D){Boid_Min_Speed, 0.0f}; 
        }
    }

    boid->position = vec_add(boid->position, vec_mult(boid->velocity, g_DeltaTime));
    */

    Vector2D total_force = calculate_Forces(boid, flock, obstacle_mgr);
    Vector2D acceleration = vec_mult(total_force, 1.0f / Boid_Mass);
    boid->velocity = vec_add(boid->velocity, vec_mult(acceleration, g_DeltaTime));

    // Enforce limits and cruise speeds
    float current_speed = vec_mag(boid->velocity);
    if (current_speed > Boid_Max_Speed) {
        boid->velocity = vec_limit(boid->velocity, Boid_Max_Speed);
    } else if (current_speed < Boid_Min_Speed) {
        if (current_speed > 0.01f) {
            boid->velocity = vec_mult(boid->velocity, Boid_Min_Speed / current_speed);
        } else {
            boid->velocity = (Vector2D){Boid_Min_Speed, 0.0f}; 
        }
    }

    boid->position = vec_add(boid->position, vec_mult(boid->velocity, g_DeltaTime));
}

Vector2D calculate_Forces(Boid* boid, Flock *flock, ObstacleManager *obstacle_mgr) {
    Vector2D align_steering_force = {0.0f, 0.0f};
    Vector2D cohesion_steering_force = {0.0f, 0.0f};
    Vector2D separation_steering_force = {0.0f, 0.0f};

    int separation_count = 0;
    int alignment_count = 0;
    int cohesion_count = 0;

    Vector2D heading = {0.0f, 0.0f};
    float current_speed = vec_mag(boid->velocity);
    if (current_speed > 0.0f) {
        heading = vec_mult(boid->velocity, 1.0f / current_speed);
    }
    // Cosine of half the vision angle: cos(270 / 2) = cos(135) ≈ -0.7071f
    float vision_threshold = cosf((Boid_Vision_Angle / 2.0f) * (M_PI / 180.0f));

    for (int i = 0; i < flock->count; i++) {
        Boid *other = &flock->boids[i];
        if (boid == other) continue; 
    
        Vector2D to_neighbor = vec_sub(other->position, boid->position);
        float distance = vec_mag(to_neighbor);
        
        if (distance > 0.0f){
            Vector2D dir_to_neighbor = vec_mult(to_neighbor, 1.0f / distance); // Normalize it

            if(vec_dot(heading, dir_to_neighbor) >= vision_threshold){
                // Neighbor is within the vision cone
                if(distance < Boid_Perception_Radius){
                    // alignment
                    align_steering_force = vec_add(align_steering_force, other->velocity);
                    alignment_count++;
                    // cohesion
                    cohesion_steering_force = vec_add(cohesion_steering_force, other->position);
                    cohesion_count++;
                }

                if(distance < Boid_Separation_Radius){
                    // separation
                    
                    // Check to see if the other boid is too close and within the separation radius
                    if(distance < Boid_Separation_Threshold){
                        // If the other boid is too close, apply a stronger separation force
                        Vector2D escape_vector = vec_mult(to_neighbor, 1.0f / distance); 
                        escape_vector = vec_mult(escape_vector, -2.0f); // Stronger push away
                        separation_steering_force = vec_add(separation_steering_force, escape_vector);
                    } else {
                        Vector2D escape_vector = vec_mult(to_neighbor, 1.0f / distance); 
                        separation_steering_force = vec_add(separation_steering_force, escape_vector);
                    }

                    separation_count++;
                }
            }
        }
    }

    if(separation_count > 0){
        separation_steering_force = vec_mult(separation_steering_force, 1.0f / separation_count);
        float sep_mag = vec_mag(separation_steering_force);
        if(sep_mag > 0.0f){
            separation_steering_force = vec_mult(separation_steering_force, 1.0f / sep_mag);
            separation_steering_force = vec_mult(separation_steering_force, Boid_Max_Speed);
        }
        separation_steering_force = vec_sub(separation_steering_force, boid->velocity);
        separation_steering_force = vec_limit(separation_steering_force, Boid_Max_Force);
    }

    if(alignment_count > 0){
        align_steering_force = vec_mult(align_steering_force, 1.0f / alignment_count);
        float align_mag = vec_mag(align_steering_force);
        if(align_mag > 0.0f){
            align_steering_force = vec_mult(align_steering_force, 1.0f / align_mag);
            align_steering_force = vec_mult(align_steering_force, Boid_Max_Speed);
        }
        align_steering_force = vec_sub(align_steering_force, boid->velocity);
        align_steering_force = vec_limit(align_steering_force, Boid_Max_Force);
    }

    if(cohesion_count > 0){
        cohesion_steering_force = vec_mult(cohesion_steering_force, 1.0f / cohesion_count);
        float cohesion_mag = vec_mag(cohesion_steering_force);
        if(cohesion_mag > 0.0f){
            cohesion_steering_force = vec_mult(cohesion_steering_force, 1.0f / cohesion_mag);
            cohesion_steering_force = vec_mult(cohesion_steering_force, Boid_Max_Speed);
        }
        cohesion_steering_force = vec_sub(cohesion_steering_force, boid->velocity);
        cohesion_steering_force = vec_limit(cohesion_steering_force, Boid_Max_Force);
    }


    Vector2D total_force = {0.0f, 0.0f};
    total_force = vec_add(total_force, vec_mult(separation_steering_force, Boid_Separation_Weight));
    total_force = vec_add(total_force, vec_mult(align_steering_force, Boid_Alignment_Weight));
    total_force = vec_add(total_force, vec_mult(cohesion_steering_force, Boid_Cohesion_Weight));

    Vector2D boundary_force = calculate_boundaries(boid);
    total_force = vec_add(total_force, boundary_force);

    Vector2D mouse_steering_force = {0.0f, 0.0f};
    if(g_IsLeftMouseDown){
        mouse_steering_force = calculate_mouse_seek(boid, g_MousePosition);
        total_force = vec_add(total_force, vec_mult(mouse_steering_force, Boid_Mouse_Seek_Weight));
    }

    Vector2D obstacle_force = calculate_obstacle_avoidance(boid, obstacle_mgr);
    total_force = vec_add(total_force, obstacle_force);

    return total_force;

}

Vector2D calculate_boundaries(Boid *boid) {
    Vector2D desired_velocity = boid->velocity; // Default to current velocity if safe
    int near_edge = 0;

    // Left Edge
    if (boid->position.x < margin) {
        desired_velocity.x = Boid_Max_Speed;
        near_edge = 1;
    }
    // Right Edge
    else if (boid->position.x > (WINDOW_WIDTH - margin)) {
        desired_velocity.x = -Boid_Max_Speed;
        near_edge = 1;
    }

    // Top Edge
    if (boid->position.y < margin) {
        desired_velocity.y = Boid_Max_Speed;
        near_edge = 1;
    }
    // Bottom Edge
    else if (boid->position.y > (WINDOW_HEIGHT - margin)) {
        desired_velocity.y = -Boid_Max_Speed;
        near_edge = 1;
    }

    Vector2D steering = {0.0f, 0.0f};
    if (near_edge) {
        // Ensure the desired velocity is normalized and scaled to max speed
        float mag = vec_mag(desired_velocity);
        if (mag > 0.0f) {
            desired_velocity = vec_mult(desired_velocity, 1.0f / mag);
            desired_velocity = vec_mult(desired_velocity, Boid_Max_Speed);
        }

        // Steering = Desired - Current
        steering = vec_sub(desired_velocity, boid->velocity);
        
        // Use a slightly stronger max force multiplier for boundaries so they turn sharply
        steering = vec_limit(steering, Boid_Max_Force * Boid_Boundary_Force); 
    }

    return steering;
}

Vector2D calculate_mouse_seek(Boid *boid, Vector2D mouse_pos) {
    // 1. Vector pointing directly from the boid to the mouse position
    Vector2D desired_velocity = vec_sub(mouse_pos, boid->position);
    
    float distance = vec_mag(desired_velocity);
    Vector2D steering = {0.0f, 0.0f};

    if (distance > 0.0f) {
        // 2. Set the desired velocity vector to fly at Max Speed toward the target
        desired_velocity = vec_mult(desired_velocity, 1.0f / distance); // Normalize
        desired_velocity = vec_mult(desired_velocity, Boid_Max_Speed);

        // 3. Steering = Desired - Current
        steering = vec_sub(desired_velocity, boid->velocity);
        steering = vec_limit(steering, Boid_Max_Force);
    }

    return steering;
}



void draw_boid(SDLContext *context, Boid *boid) {
    SDL_SetTextureColorMod(context->particle_asset.texture, boid->r, boid->g, boid->b);
    SDL_SetTextureAlphaMod(context->particle_asset.texture, boid->a);
    SDL_FRect dst = { boid->position.x - boid->size * 0.5f, boid->position.y - boid->size * 0.5f, boid->size, boid->size };
    SDL_RenderTexture(context->renderer, context->particle_asset.texture, NULL, &dst);
    
    SDL_SetTextureColorMod(context->particle_asset.texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(context->particle_asset.texture, 255);
}
