#include "general.h"


float getRandomFloat(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

Vector2D vec_add(Vector2D v1, Vector2D v2) {
    return (Vector2D){v1.x + v2.x, v1.y + v2.y};
}

Vector2D vec_sub(Vector2D v1, Vector2D v2) {
    return (Vector2D){v1.x - v2.x, v1.y - v2.y};
}

Vector2D vec_mult(Vector2D v, float scalar) {
    return (Vector2D){v.x * scalar, v.y * scalar};
}

float vec_mag(Vector2D v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

float vec_dot(Vector2D v1, Vector2D v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

Vector2D vec_limit(Vector2D v, float max_val) {
    float mag = vec_mag(v);
    if (mag > max_val && mag > 0.0f) {
        float scale = max_val / mag;
        return vec_mult(v, scale);
    }
    return v;
}

