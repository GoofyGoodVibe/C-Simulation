#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL3/SDL.h>
#include "assets.h"

#define MAX_PARTICLE_COUNT 175

typedef struct {
    float x, y;
    float vx, vy;
    float ax, ay; // CRITICAL: Holds accumulated forces before moving
    float life;
    float size;
    Uint8 r, g, b, a;

    // Kept strictly for Debugging / Visualization
    float coh_x, coh_y;
    float sep_x, sep_y;
    float alg_x, alg_y;

    // Perception constraints
    float visual_radius; // e.g., 75.0f
} Particle;

typedef struct {
    Particle particles[MAX_PARTICLE_COUNT];
    int count;
} ParticleSystem;

typedef struct { // Structure to pass to worker threads for behvaior calculations
    ParticleSystem *system;
    int start_idx;
    int end_idx;
    int window_width;
    int window_height;
} BehaviorTaskData;

typedef struct { // Structure to pass to worker threads for collision resolution
    ParticleSystem *system;
    int start_idx;
    int end_idx;
    int window_width;
    int window_height;
} CollisionTaskData;

typedef struct { // completion tracking for synchronization
    int completed_tasks;
    SDL_Mutex *mutex;
    SDL_Condition *condition;
    int total_tasks;
} SyncCounter;

ParticleSystem particle_system_init();
Particle particle_create(float x, float y, float vx, float vy, float size, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void particles_add(Particle particle, ParticleSystem *system);
void particles_draw(SDL_Renderer *renderer, const ParticleSystem *system, const Asset *particle_asset);
void particles_setup(int window_width, int window_height, ParticleSystem *particle_system);
void particles_update(ParticleSystem *system, float delta_time,int window_width, int window_height);

void particles_threading_init(void);
void particles_threading_cleanup(void);
void particles_update_behaviors_threaded(ParticleSystem *system, int window_width, int window_height);
void resolve_collisions_threaded(ParticleSystem *sys, int window_width, int window_height);
void behavior_worker(void *arg);
void collision_worker(void *arg);


#endif // PARTICLE_H