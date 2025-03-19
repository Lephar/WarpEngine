#pragma once

#include "pch.h"

typedef enum contentType ContentType;

typedef struct material Material;

struct primitive {
    ContentType *type;
    Material *material;
    VkDeviceSize indexBegin;
    VkDeviceSize indexCount;
    VkDeviceSize vertexOffset;
    VkDeviceSize uniformOffset;
} typedef Primitive;

extern const uint32_t primitiveCountLimit;
extern uint32_t primitiveCount;
extern Primitive *primitives;

void loadAsset(ContentType type, const char *assetName);
