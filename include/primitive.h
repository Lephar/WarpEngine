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

cgltf_data *loadAsset(const char *assetName);
void processAsset(cgltf_data *data);
void freeAsset(cgltf_data *data);
