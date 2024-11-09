#include "zero.h"

typedef uint32_t Index;

struct vertex {
    float x;
    float y;
    float z;
} typedef Vertex;

struct uniform {
    float transform[4][4];
} typedef Uniform;

void initializeAssets();
void loadAssets();
void freeAssets();