#pragma once

#include "pch.h"
#include "file.h"
#include "image.h"

// TODO: Use data oriented design for content instead of object oriented

struct protoTexture {
    VkExtent3D extent;
    Data data;
} typedef ProtoTexture;

struct protoMaterial {
    char name[UINT8_MAX];
    ProtoTexture normal;
    ProtoTexture baseColor;
} typedef ProtoMaterial;

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct material {
    char name[UINT8_MAX];
    Image texture;
    Image normal;
    Image baseColor;
} typedef Material;

struct primitive {
    cgltf_size indexCount;
    Index *indices;
    cgltf_size vertexCount;
    Vertex *vertices;
    Material *material;
} typedef Primitive;

struct mesh {
    cgltf_size primitiveCount;
    Primitive *primitives;
} typedef Mesh;

struct node {
    mat4 transform;
    cgltf_bool hasMesh;
    Mesh mesh;
    cgltf_size childCount;
    struct node *children;
} typedef Node;

struct scene {
    cgltf_size nodeCount;
    Node *nodes;
} typedef Scene;

struct asset {
    cgltf_size materialCount;
    ProtoMaterial *materials;
    cgltf_size sceneCount;
    Scene *scenes;
} typedef Asset;

struct uniform {
    mat4 view;
    mat4 proj;
    mat4 camera;
} typedef Uniform;

void loadAssets();
void moveAssets();
