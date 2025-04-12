#pragma once

#include "pch.h"

typedef struct material Material;

struct primitive {
    Material *material;
    uint32_t indexBegin;
    uint32_t indexCount;
    int32_t  vertexOffset;
    uint32_t uniformOffset;
} typedef Primitive;

extern uint32_t primitiveCount;
extern Primitive *primitives;

void loadPrimitive(Primitive *primitive, cgltf_primitive *primitiveData, mat4 transform);
void drawPrimitive(VkCommandBuffer commandBuffer, VkDescriptorSet descriptorSet, Primitive *primitive);
