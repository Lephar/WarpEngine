#pragma once

#include "pch.h"

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct sceneUniform {
    mat4 anchor;
    mat4 camera;
} typedef SceneUniform;

struct primitiveUniform {
    mat4 transform;
} typedef PrimitiveUniform;

extern uint64_t indexCount;
extern uint64_t vertexCount;

extern uint64_t indexBufferSize;
extern uint64_t vertexBufferSize;
extern uint64_t uniformBufferSize;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;
extern void   *uniformBuffer;

void createContentBuffers();
void loadContent();
void destroyContentBuffers();
void freeContent();
