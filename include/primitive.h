#pragma once

#include "pch.h"

#include "content.h"

typedef struct material Material;

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct primitive {
    ContentType type;
    Material *material;
    VkDeviceSize indexBegin;
    VkDeviceSize indexCount;
    VkDeviceSize vertexOffset;
    VkDeviceSize uniformOffset;
} typedef Primitive;

void loadAsset(ContentType type, const char *assetName);
