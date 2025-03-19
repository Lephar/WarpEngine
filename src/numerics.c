#include "numerics.h"

int32_t compareFloat(float first, float second) {
    float difference = first - second;

    return (difference > EPSILON) - (difference < -EPSILON);
}

float radians(float degrees) {
    return M_PI * degrees / 180.0f;
}
