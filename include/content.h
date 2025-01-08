#pragma once

#include "pch.h"

typedef uint32_t Index;

typedef vec3 Vertex;

typedef mat4 Uniform;

struct model {
    uint32_t binary;
    char name[PATH_MAX];
    char fullpath[PATH_MAX];

} typedef Model;

void initializeAssets();
void loadAssets();
void freeAssets();