#pragma once

#include "pch.h"

enum contentType {
    CAMERA,
    LIGHT,
    CUBEMAP,
    STATIONARY,
    MOVABLE,
    PORTAL
} typedef ContentType;

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct uniform {
    mat4 skyboxView;
    mat4 view;
    mat4 proj;
    mat4 skyboxCamera;
    mat4 camera;
    vec3 ambientLight;
} typedef Uniform;

extern uint64_t indexCount;
extern uint64_t vertexCount;
extern uint64_t indexBufferSize;
extern uint64_t vertexBufferSize;
extern uint64_t uniformBufferSize;

extern Index *indexBuffer;
extern Vertex *vertexBuffer;
extern Uniform *uniformBuffer;

void loadContent();
void freeContent();
