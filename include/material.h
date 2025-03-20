#pragma once

#include "pch.h"

typedef struct image Image;

struct material {
    char name[UINT8_MAX];
    Image *baseColor;
    VkDescriptorSet samplerDescriptor;
} typedef Material;

extern const uint32_t textureSizeMaxDimensionLimit;
extern const uint32_t materialCountLimit;
extern uint32_t materialCount;
extern Material *materials;

Image *loadTexture(const char *path);
void createDescriptor(Material *material);
void loadMaterial(cgltf_material *materialData);
void destroyMaterial(Material *material);
