#pragma once

#include "pch.h"

typedef struct material Material;

struct primitive {
    Material *material;
    VkDeviceSize indexBegin;
    VkDeviceSize indexCount;
    VkDeviceSize vertexOffset;
    VkDeviceSize uniformOffset;
} typedef Primitive;

extern const uint32_t primitiveCountLimit;
extern uint32_t primitiveCount;
extern Primitive *primitives;

void loadPrimitive(Primitive *primitive, cgltf_primitive *primitiveData, mat4 transform);
