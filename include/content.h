#pragma once

#include "pch.h"
#include "image.h"

// TODO: Use data oriented design for content instead of object oriented

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec3 texcoord;
} typedef Vertex;

struct material {
    Image texture;
    Image normal;
    Image tangent; // TODO: What is this?
} typedef Material;

struct primitive {
    Index *indices;
    Vertex *vertices;
    Material material;
} typedef Primitive;

struct mesh {
    cgltf_size primitiveCount;
    Primitive *primitive;
} typedef Mesh;

struct node {
    mat4 transform;
    cgltf_size meshCount;
    Mesh *meshes;
} typedef Node;

struct model {
    cgltf_size nodeCount;
    Node *nodes;
} typedef Model;

struct scene {
    uint32_t binary;
    char name[PATH_MAX];
    char fullpath[PATH_MAX];
} typedef Scene;

struct uniform {
    mat4 view;
    mat4 proj;
    mat4 camera;
} typedef Uniform;

void initializeAssets();
void loadAssets();
void freeAssets();
