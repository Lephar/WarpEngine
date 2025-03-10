#pragma once

#include "pch.h"
#include "file.h"
#include "image.h"

enum assetType {
    CAMERA,
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
    vec3 ambientLight;
} typedef Uniform;

struct material {
    char name[UINT8_MAX];
    Image baseColor;
    VkDescriptorSet samplerDescriptor;
} typedef Material;

struct drawable {
    AssetType type;
    Material *material;
    VkDeviceSize indexBegin;
    VkDeviceSize indexCount;
    VkDeviceSize vertexOffset;
    VkDeviceSize uniformOffset;
} typedef Drawable;

void loadAssets();
void freeAssets();
