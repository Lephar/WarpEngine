#pragma once

#include "pch.h"
#include "file.h"
#include "image.h"

enum assetType {
    CUBEMAP,
    STATIONARY,
    MOVABLE,
    PORTAL
} typedef AssetType;

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
} typedef Uniform;

struct material {
    char name[UINT8_MAX];
    Image baseColor;
    VkSampler sampler;
    VkDescriptorSet descriptor;
} typedef Material;

struct drawable {
    VkDeviceSize indexBegin;
    VkDeviceSize indexCount;
    VkDeviceSize vertexOffset;
    VkDescriptorSet *descriptorReference;
} typedef Drawable;

void loadAssets();
void freeAssets();
