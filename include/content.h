#pragma once

#include "pch.h"
#include "file.h"
#include "image.h"

// TODO: Use data oriented design for content instead of object oriented

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct uniform {
    mat4 view;
    mat4 proj;
    mat4 camera;
} typedef Uniform;

struct textureInfo {
    VkExtent3D extent;
    VkDeviceSize offset;
    VkDeviceSize size;
} typedef TextureInfo;

struct material {
    char name[UINT8_MAX];
    TextureInfo baseColorInfo;
    Image baseColor;
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
