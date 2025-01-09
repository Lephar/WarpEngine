#pragma once

#include "pch.h"

typedef uint32_t Index;

struct material {

} typedef Material;

struct vertex {
    vec3 position;
    vec3 texcoord;
} typedef Vertex;

struct mesh {
    Index *indices;
    Vertex *vertices;
    mat4 transform;
    Material normal;
    Material texture;
} typedef Mesh;

struct uniform {
    mat4 view;
    mat4 proj;
    mat4 camera;
} typedef Uniform;

struct model {
    uint32_t binary;
    char name[PATH_MAX];
    char fullpath[PATH_MAX];

} typedef Model;

void initializeAssets();
void loadAssets();
void freeAssets();
