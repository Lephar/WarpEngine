#include "numerics.h"

// NOTICE: These min/max macros are for internal use only.
// Separate functions prevent double-evaluation and guarantee type-safety.

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

// END OF NOTICE

float radians(float degrees) {
    return M_PI * degrees / 180.0f;
}

int32_t compareFloat(float first, float second) {
    float difference = first - second;

    return (difference > EPSILON) - (difference < -EPSILON);
}

// TODO: Change to stdc_count_ones when ready
uint32_t popcount(uint32_t value) {
    uint32_t count = 0;

    while(value) {
        count += (value & 1);
        value >>= 1;
    }

    return count;
}

uint64_t align(uint64_t value, uint64_t alignment) {
    return (value + alignment - 1) / alignment * alignment;
}

uint64_t alignBack(uint64_t value, uint64_t alignment) {
    return value / alignment * alignment;
}

int8_t bmin(int8_t first, int8_t second) {
    return min(first, second);
}

int8_t bmax(int8_t first, int8_t second) {
    return max(first, second);
}

uint8_t ubmin(uint8_t first, uint8_t second) {
    return min(first, second);
}

uint8_t ubmax(uint8_t first, uint8_t second) {
    return max(first, second);
}

int16_t smin(int16_t first, int16_t second) {
    return min(first, second);
}

int16_t smax(int16_t first, int16_t second) {
    return max(first, second);
}

uint16_t usmin(uint16_t first, uint16_t second) {
    return min(first, second);
}

uint16_t usmax(uint16_t first, uint16_t second) {
    return max(first, second);
}

int32_t imin(int32_t first, int32_t second) {
    return min(first, second);
}

int32_t imax(int32_t first, int32_t second) {
    return max(first, second);
}

uint32_t umin(uint32_t first, uint32_t second) {
    return min(first, second);
}

uint32_t umax(uint32_t first, uint32_t second) {
    return max(first, second);
}

int64_t lmin(int64_t first, int64_t second) {
    return min(first, second);
}

int64_t lmax(int64_t first, int64_t second) {
    return max(first, second);
}

uint64_t ulmin(uint64_t first, uint64_t second) {
    return min(first, second);
}

uint64_t ulmax(uint64_t first, uint64_t second) {
    return max(first, second);
}
