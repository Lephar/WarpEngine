#pragma once

#include "pch.h"

typedef uint32_t Index;

struct vertex {
    float x;
    float y;
    float z;
} typedef Vertex;

struct uniform {
    mat4 transform;
} typedef Uniform;

struct model {
    uint32_t binary;
    char name[PATH_MAX];
    char fullpath[PATH_MAX];

} typedef Model;

void initializeAssets();
void loadAssets();
void freeAssets();