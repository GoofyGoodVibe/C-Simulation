#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "particle.h"
#include "threadpool.h"
#include "general.h"
#include "simtimer.h"

static SyncCounter behavior_sync = {0, NULL, NULL, 0};
static SyncCounter collision_sync = {0, NULL, NULL, 0};

static void wrapped_delta(float a, float b, float wrap_size, float *delta);

static bool debug = false; // Set to true to enable debug output

ParticleSystem particle_system_init() {
    ParticleSystem system;
    system.count = 0;
    return system;
}

Particle particle_create(float x, float y, float vx, float vy, float size, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    Particle particle;
    particle.x = x;
    particle.y = y;
    particle.vx = vx;
    particle.vy = vy;
    particle.ax = 0.0f; // Reset acceleration vectors explicitly
    particle.ay = 0.0f; // Reset acceleration vectors explicitly
    particle.size = size;
    particle.r = r;
    particle.g = g;
    particle.b = b;
    particle.a = a;
    
    // Debug variables
    particle.coh_x = 0.0f; particle.coh_y = 0.0f;
    particle.sep_x = 0.0f; particle.sep_y = 0.0f;
    particle.alg_x = 0.0f; particle.alg_y = 0.0f;

    // CRITICAL FIX: Give them sight right out of the gate!
    particle.visual_radius = 85.0f; 

    return particle;
}

void particles_add(Particle particle, ParticleSystem *system) {
    if (system->count < MAX_PARTICLE_COUNT) {
        system->particles[system->count] = particle;
        system->count++;
    }
}

void particles_draw(SDL_Renderer *renderer, const ParticleSystem *system, const Asset *particle_asset) {
    for (int i = 0; i < system->count; i++) {
        const Particle *p = &system->particles[i];
        SDL_SetTextureColorMod(particle_asset->texture, p->r, p->g, p->b);
        SDL_SetTextureAlphaMod(particle_asset->texture, p->a);
        SDL_FRect dst = { p->x - p->size * 0.5f, p->y - p->size * 0.5f, p->size, p->size };
        SDL_RenderTexture(renderer, particle_asset->texture, NULL, &dst);
        
        SDL_SetTextureColorMod(particle_asset->texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(particle_asset->texture, 255);

        if (debug) {
            float coh_len = sqrtf(p->coh_x * p->coh_x + p->coh_y * p->coh_y);
            if (coh_len > 0.0001f) {
                float target_x = p->x + (p->coh_x / coh_len) * LINE_LENGTH;
                float target_y = p->y + (p->coh_y / coh_len) * LINE_LENGTH;
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
                SDL_RenderLine(renderer, p->x, p->y, target_x, target_y);
            }

            float sep_len = sqrtf(p->sep_x * p->sep_x + p->sep_y * p->sep_y);
            if (sep_len > 0.0001f) {
                float target_x = p->x + (p->sep_x / sep_len) * LINE_LENGTH;
                float target_y = p->y + (p->sep_y / sep_len) * LINE_LENGTH;
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
                SDL_RenderLine(renderer, p->x, p->y, target_x, target_y);
            }

            float alg_len = sqrtf(p->alg_x * p->alg_x + p->alg_y * p->alg_y);
            if (alg_len > 0.0001f) {
                float target_x = p->x + (p->alg_x / alg_len) * LINE_LENGTH;
                float target_y = p->y + (p->alg_y / alg_len) * LINE_LENGTH;
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); 
                SDL_RenderLine(renderer, p->x, p->y, target_x, target_y);
            }
        }
    }
}

void particles_setup(int window_width, int window_height, ParticleSystem *particle_system) {
    for(int i = 0; i < MAX_PARTICLE_COUNT; i++) {
        // Lower initial velocities (-50 to +50) so steering forces can take over instantly
        float initial_vx = (float)((rand() % 100) - 50);
        float initial_vy = (float)((rand() % 100) - 50);

        Particle p = particle_create(
            (float)(rand() % window_width), 
            (float)(rand() % window_height), 
            initial_vx, 
            initial_vy, 
            PARTICLE_RADIUS, 
            (Uint8)(rand() % 256), 
            (Uint8)(rand() % 256), 
            (Uint8)(rand() % 256), 
            255 // Set alpha to fully opaque so they are clearly visible
        );

        p.visual_radius = 80.0f; 

        particles_add(p, particle_system);
    }
}

void particles_update(ParticleSystem *system, float delta_time, int window_width, int window_height) {
    
     // Phase 1: Parallel behavior calculation (DYNAMIC worker count)
    particles_update_behaviors_threaded(system, window_width, window_height);

    for (int i = 0; i < system->count; i++) {
        Particle *p = &system->particles[i];

        // Apply forces to velocity via acceleration
        p->vx += p->ax * delta_time;
        p->vy += p->ay * delta_time;

        // Smooth speed scaling to MAX_SPEED
        float current_speed = sqrtf(p->vx * p->vx + p->vy * p->vy);
        float dynamic_max = MAX_SPEED * (SPEED_VARIANCE_MIN + (float)(i % SPEED_VARIANCE_CLASSES) * SPEED_VARIANCE_INCREMENT);

        if (current_speed > dynamic_max) {
            p->vx = (p->vx / current_speed) * dynamic_max;
            p->vy = (p->vy / current_speed) * dynamic_max;
            current_speed = dynamic_max;
        }

        // Momentum preservation using your threshold limit scaled by delta_time
        if (current_speed < MOMENTUM_THRESHOLD && current_speed > 0.0001f) {
            float glide = MOMENTUM_ACCELERATION * delta_time;
            p->vx += (p->vx / current_speed) * glide;
            p->vy += (p->vy / current_speed) * glide;
        }

        // Move positions
        p->x += p->vx * delta_time;
        p->y += p->vy * delta_time;
    }

    // 2. Clear overlapping positions
    resolve_collisions_threaded(system, window_width, window_height);

    // 3. Final single-point screen wrap logic
    for (int i = 0; i < system->count; i++) {
        Particle *p = &system->particles[i];
        if (p->x < 0.0f) p->x += (float)window_width;
        if (p->x >= (float)window_width) p->x -= (float)window_width;
        if (p->y < 0.0f) p->y += (float)window_height;
        if (p->y >= (float)window_height) p->y -= (float)window_height;
    }
}

static void wrapped_delta(float a, float b, float wrap_size, float *delta) {
    float d = a - b;
    if (d > wrap_size * 0.5f)  d -= wrap_size;
    if (d < -wrap_size * 0.5f) d += wrap_size;
    *delta = d;
}
 
// Worker thread function for behavior calculation
void behavior_worker(void *arg) {
    BehaviorTaskData *task_data = (BehaviorTaskData *)arg;
    ParticleSystem *system = task_data->system;
    int start_idx = task_data->start_idx;
    int end_idx = task_data->end_idx;
    int window_width = task_data->window_width;
    int window_height = task_data->window_height;
    float inv_dt = (g_DeltaTime > 0.0f) ? 1.0f / g_DeltaTime : 1.0f;
 
    // Process assigned particle range
    for (int i = start_idx; i < end_idx; i++) {
        Particle *a = &system->particles[i];
        
        float cx = 0.0f, cy = 0.0f;
        float sx = 0.0f, sy = 0.0f;
        float jx = 0.0f, jy = 0.0f;
 
        int coh_count = 0;
        int alg_count = 0;
 
        float speed = sqrtf(a->vx * a->vx + a->vy * a->vy);
        float hx = (speed > 0.001f) ? a->vx / speed : 0.0f;
        float hy = (speed > 0.001f) ? a->vy / speed : 0.0f;
 
        // Check all other particles for neighbor behaviors
        for (int j = 0; j < system->count; j++) {
            if (i == j) continue;
            Particle *b = &system->particles[j];
 
            float dx, dy;
            wrapped_delta(b->x, a->x, (float)window_width, &dx);
            wrapped_delta(b->y, a->y, (float)window_height, &dy);
            float dist = sqrtf(dx * dx + dy * dy);
 
            if (dist <= 0.0f || dist > a->visual_radius) continue;
 
            float nx = dx / dist;
            float ny = dy / dist;
            float dot_product = (hx * nx) + (hy * ny);
 
            if (dot_product < FORWARD_DOT_THRESHOLD) continue;
 
            cx += dx; 
            cy += dy;
            coh_count++;
 
            jx += b->vx;
            jy += b->vy;
            alg_count++;
 
            if (dist < SEPARATION_RADIUS) {
                sx -= dx / dist;
                sy -= dy / dist;
            }
        }
 
        if (coh_count > 0) { cx /= coh_count; cy /= coh_count; }
        if (alg_count > 0) { jx /= alg_count; jy /= alg_count; }
 
        a->coh_x = cx * COHESION_STRENGTH * inv_dt;
        a->coh_y = cy * COHESION_STRENGTH * inv_dt;
        a->alg_x = jx * ALIGNMENT_STRENGTH * inv_dt;
        a->alg_y = jy * ALIGNMENT_STRENGTH * inv_dt;
        a->sep_x = sx * SEPARATION_STRENGTH * inv_dt;
        a->sep_y = sy * SEPARATION_STRENGTH * inv_dt;
 
        float total_fx = a->coh_x + a->alg_x + a->sep_x;
        float total_fy = a->coh_y + a->alg_y + a->sep_y;
 
        float force_mag = sqrtf(total_fx * total_fx + total_fy * total_fy);
        if (force_mag > MAX_FORCE) {
            total_fx = (total_fx / force_mag) * MAX_FORCE;
            total_fy = (total_fy / force_mag) * MAX_FORCE;
        }
 
        a->ax = total_fx;
        a->ay = total_fy;
    }
 
    // Signal completion
    SDL_LockMutex(behavior_sync.mutex);
    behavior_sync.completed_tasks++;
    if (behavior_sync.completed_tasks >= behavior_sync.total_tasks) {
        SDL_BroadcastCondition(behavior_sync.condition);
    }
    SDL_UnlockMutex(behavior_sync.mutex);
 
    free(task_data);
}
 
// Worker thread function for collision resolution
void collision_worker(void *arg) {
    CollisionTaskData *task_data = (CollisionTaskData *)arg;
    ParticleSystem *system = task_data->system;
    int start_idx = task_data->start_idx;
    int end_idx = task_data->end_idx;
    int window_width = task_data->window_width;
    int window_height = task_data->window_height;
 
    // Each thread checks its particles against all others
    for (int i = start_idx; i < end_idx; i++) {
        Particle *a = &system->particles[i];
        for (int j = 0; j < system->count; j++) {
            if (i == j) continue;
            Particle *b = &system->particles[j];
            
            float dx, dy;
            wrapped_delta(b->x, a->x, (float)window_width, &dx);
            wrapped_delta(b->y, a->y, (float)window_height, &dy);
            
            float dist2 = dx * dx + dy * dy;
            float minDist = (a->size + b->size) * 0.5f; 
            float minDist2 = minDist * minDist;
            
            if (dist2 > 0.0f && dist2 < minDist2) {
                float dist = sqrtf(dist2);
                float overlap = 0.5f * (minDist - dist);
                
                float nx = dx / dist;
                float ny = dy / dist;
 
                a->x -= nx * overlap;
                a->y -= ny * overlap;
            }
        }
    }
 
    // Signal completion
    SDL_LockMutex(collision_sync.mutex);
    collision_sync.completed_tasks++;
    if (collision_sync.completed_tasks >= collision_sync.total_tasks) {
        SDL_BroadcastCondition(collision_sync.condition);
    }
    SDL_UnlockMutex(collision_sync.mutex);
 
    free(task_data);
}
 
// Initialize synchronization primitives
void particles_threading_init(void) {
    if (behavior_sync.mutex == NULL) {
        behavior_sync.mutex = SDL_CreateMutex();
        behavior_sync.condition = SDL_CreateCondition();
        behavior_sync.completed_tasks = 0;
        behavior_sync.total_tasks = 0;
    }
    if (collision_sync.mutex == NULL) {
        collision_sync.mutex = SDL_CreateMutex();
        collision_sync.condition = SDL_CreateCondition();
        collision_sync.completed_tasks = 0;
        collision_sync.total_tasks = 0;
    }
}
 
// Cleanup synchronization primitives
void particles_threading_cleanup(void) {
    if (behavior_sync.mutex) {
        SDL_DestroyMutex(behavior_sync.mutex);
        SDL_DestroyCondition(behavior_sync.condition);
        behavior_sync.mutex = NULL;
    }
    if (collision_sync.mutex) {
        SDL_DestroyMutex(collision_sync.mutex);
        SDL_DestroyCondition(collision_sync.condition);
        collision_sync.mutex = NULL;
    }
}
 
// DYNAMIC: Get actual worker count from threadpool
void particles_update_behaviors_threaded(ParticleSystem *system, int window_width, int window_height) {
    int num_workers = g_pool.num_workers;  // ← DYNAMIC
    int particles_per_worker = (system->count + num_workers - 1) / num_workers;
 
    // Reset synchronization counter
    SDL_LockMutex(behavior_sync.mutex);
    behavior_sync.completed_tasks = 0;
    behavior_sync.total_tasks = num_workers;
    SDL_UnlockMutex(behavior_sync.mutex);
 
    // Queue behavior tasks for all workers
    for (int i = 0; i < num_workers; i++) {
        int start_idx = i * particles_per_worker;
        int end_idx = (i + 1) * particles_per_worker;
        if (end_idx > system->count) end_idx = system->count;
        
        if (start_idx >= system->count) break;
 
        BehaviorTaskData *task_data = (BehaviorTaskData *)malloc(sizeof(BehaviorTaskData));
        task_data->system = system;
        task_data->start_idx = start_idx;
        task_data->end_idx = end_idx;
        task_data->window_width = window_width;
        task_data->window_height = window_height;
 
        pool_push_task(behavior_worker, (void *)task_data);
    }
 
    // Wait for all behavior tasks to complete
    SDL_LockMutex(behavior_sync.mutex);
    while (behavior_sync.completed_tasks < behavior_sync.total_tasks) {
        SDL_WaitCondition(behavior_sync.condition, behavior_sync.mutex);
    }
    SDL_UnlockMutex(behavior_sync.mutex);
}
 
// DYNAMIC: Get actual worker count from threadpool
void resolve_collisions_threaded(ParticleSystem *sys, int window_width, int window_height) {
    int num_workers = g_pool.num_workers;  // ← DYNAMIC
    int particles_per_worker = (sys->count + num_workers - 1) / num_workers;
 
    // Reset synchronization counter
    SDL_LockMutex(collision_sync.mutex);
    collision_sync.completed_tasks = 0;
    collision_sync.total_tasks = num_workers;
    SDL_UnlockMutex(collision_sync.mutex);
 
    // Queue collision tasks for all workers
    for (int i = 0; i < num_workers; i++) {
        int start_idx = i * particles_per_worker;
        int end_idx = (i + 1) * particles_per_worker;
        if (end_idx > sys->count) end_idx = sys->count;
        
        if (start_idx >= sys->count) break;
 
        CollisionTaskData *task_data = (CollisionTaskData *)malloc(sizeof(CollisionTaskData));
        task_data->system = sys;
        task_data->start_idx = start_idx;
        task_data->end_idx = end_idx;
        task_data->window_width = window_width;
        task_data->window_height = window_height;
 
        pool_push_task(collision_worker, (void *)task_data);
    }
 
    // Wait for all collision tasks to complete
    SDL_LockMutex(collision_sync.mutex);
    while (collision_sync.completed_tasks < collision_sync.total_tasks) {
        SDL_WaitCondition(collision_sync.condition, collision_sync.mutex);
    }
    SDL_UnlockMutex(collision_sync.mutex);
}