#pragma once

#include "pch.h"

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

extern uint64_t indexCount;
extern uint64_t vertexCount;

extern uint64_t indexBufferSize;
extern uint64_t vertexBufferSize;
extern uint64_t uniformBufferSize;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;
extern void   *uniformBuffer;

void loadContent();
void freeContent();
