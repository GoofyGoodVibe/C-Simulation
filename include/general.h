#ifndef GENERAL_H
#define GENERAL_H

#include <SDL3/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} SDLContext;
#include "particle.h"

SDLContext InitSDL(int window_width, int window_height);
void Update(ParticleSystem *particle_system, float delta_time, int window_width, int window_height);
void Draw(SDL_Renderer *renderer, ParticleSystem *particle_system, Asset *particle_asset);
void StartUp();

// =============================================================================
// STEERING FORCES (How boids interact with neighbors)
// =============================================================================
 
// How strongly boids are attracted to the center of their flock
// Range: 0.0 (no cohesion) to 1.0 (very strong)
// Higher = boids cluster tighter together
static const float COHESION_STRENGTH = 0.04f;
 
// How much boids try to align their velocity with neighbors
// Range: 0.0 (ignore neighbors) to 1.0 (copy neighbor velocity perfectly)
// Higher = more uniform flock behavior, lower = snaking/weaving variations
static const float ALIGNMENT_STRENGTH = 0.15f;
 
// How strongly boids push away from each other to avoid crowding
// Range: 0.0 (no separation) to 2.0 (very aggressive)
// Higher = boids maintain more personal space
static const float SEPARATION_STRENGTH = 0.85f;
 
// =============================================================================
// VELOCITY & FORCE LIMITS
// =============================================================================
 
// Maximum force applied to any boid per frame (prevents extreme snapping)
// Range: 50.0 to 500.0
// Higher = boids can change direction more sharply
static const float MAX_FORCE = 150.0f;
 
// Maximum speed boids can travel
// Range: 2.0 to 20.0
// Higher = faster, more chaotic movement; lower = sluggish movement
static const float MAX_SPEED = 8.0f;
 
// =============================================================================
// FIELD OF VIEW (Perception constraints)
// =============================================================================
 
// Boids can only see neighbors in front of them (field of view cone)
// This is the dot product threshold for the forward direction
// Range: 0.0 (can see all around) to 1.0 (can only see directly ahead)
// 0.35 means ~70 degree field of view cone (reasonable for steering)
// Lower = wider vision; higher = narrower vision/tunnel vision
static const float FORWARD_DOT_THRESHOLD = 0.35f;
 
// =============================================================================
// SPATIAL DETECTION RANGES
// =============================================================================
 
// How far boids can see their neighbors (visual range)
// Range: 20.0 to 200.0
// Higher = boids are aware of distant neighbors; lower = local clustering
// Typical: 80-100 for good balance
static const float VISUAL_RADIUS = 85.0f;
 
// Particle size in pixels
// Range: 5.0 to 20.0
// Used for rendering and collision detection
static const float PARTICLE_RADIUS = 10.0f;
 
// Distance at which collision separation triggers
// Usually = PARTICLE_RADIUS × (1.5 to 4.0)
// Higher = larger personal space bubble
static const float SEPARATION_RADIUS = PARTICLE_RADIUS * 3.5f;
 
// =============================================================================
// MOMENTUM & GLIDING (How boids decelerate and glide)
// =============================================================================
 
// Speed threshold below which boids "glide" to maintain momentum
// If particle moves slower than this, it gets a push to keep moving
// Range: 20.0 to 100.0 (in terms of MAX_SPEED)
// 40.0 = ~5x MAX_SPEED; triggers at very low speeds
// Higher = glide more often; lower = less gliding (more stopping)
static const float MOMENTUM_THRESHOLD = 40.0f;
 
// How aggressively boids glide when below momentum threshold
// Applied as: glide = MOMENTUM_ACCELERATION * delta_time
// Range: 50.0 to 300.0
// ~50-80:  Sluggish, floaty feel
// ~120:    Smooth, natural feel (recommended)
// ~150+:   Zippy, energetic feel
// Higher = boids maintain speed longer; lower = faster deceleration
static const float MOMENTUM_ACCELERATION = 120.0f;
 
// Minimum speed multiplier for variation between particles
// Creates "fast" and "slow" boids in the flock
// Range: 0.5f to 1.0f
// 0.8 = slowest boids move at 80% of MAX_SPEED
// 1.0 = all boids same speed (no variation)
// Lower = more speed variation (more chaotic)
static const float SPEED_VARIANCE_MIN = 0.8f;
 
// Speed increment between different boid speed classes
// Combined with SPEED_VARIANCE_MIN to create variation
// Range: 0.05f to 0.3f
// 0.1 with modulo-5 creates 5 speed classes (0.8, 0.9, 1.0, 1.1, 1.2)
// Higher = more aggressive variation
static const float SPEED_VARIANCE_INCREMENT = 0.1f;
 
// Number of speed classes (how many different max speeds exist)
// Used with modulo: (particle_index % SPEED_VARIANCE_CLASSES)
// Divides particles into this many speed tiers
// 2 = fast/slow; 3 = fast/medium/slow; 5 = more granular (current)
// With 175 particles and 5 classes = 35 particles per class
static const int SPEED_VARIANCE_CLASSES = 5;
 
// =============================================================================
// PARTICLE INITIALIZATION
// =============================================================================
 
// Starting velocity range for particles (-INITIAL_VELOCITY_RANGE to +INITIAL_VELOCITY_RANGE)
// Range: 10.0 to 100.0
// Higher = particles start with more chaotic/random movement
static const float INITIAL_VELOCITY_RANGE = 50.0f;
 
// Minimum velocity threshold (below this = particle is effectively stopped)
// Keep this very small to avoid stopping boids completely
// Range: 0.00001f to 0.01f
// Prevents division by zero and "stuck" particles
static const float VELOCITY_EPSILON = 0.0001f;
 
// =============================================================================
// DEBUG & VISUALIZATION
// =============================================================================
 
// Length of debug lines showing force vectors
// Range: 5.0 to 30.0
// Used only when debug mode is enabled; doesn't affect simulation
static const float LINE_LENGTH = 15.0f;

#endif // GENERAL_H