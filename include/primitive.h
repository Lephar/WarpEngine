#pragma once

#include "pch.h"

typedef struct material Material;

typedef struct primitiveUniform {
    mat4 model;
} PrimitiveUniform, *PPrimitiveUniform;

typedef struct primitive {
    Material *material;
    uint32_t indexBegin;
    uint32_t indexCount;
    int32_t  vertexOffset;
    uint32_t uniformOffset;
} Primitive, *PPrimitive;

extern uint32_t primitiveCountLimit;
extern uint32_t primitiveCount;
extern Primitive *primitives;
extern PrimitiveUniform *primitiveUniforms;

uint32_t loadPrimitive(cgltf_primitive *primitiveData);
void drawPrimitive(uint32_t framebufferSetIndex, uint32_t framebufferIndex, Primitive *primitive);
