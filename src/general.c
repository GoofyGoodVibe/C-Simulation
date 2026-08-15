#include "general.h"

float getRandomFloat(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}
void limitvector(Vector2D* vector, float max) {
    float mag_sq = vector->x * vector->x + vector->y * vector->y;
    if (mag_sq > max * max) {
        float mag = sqrtf(mag_sq);
        if (mag > 0.00001f) {
            vector->x = (vector->x / mag) * max;
            vector->y = (vector->y / mag) * max;
        }
    }
}
