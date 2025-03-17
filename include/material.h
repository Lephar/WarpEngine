#pragma once

#include "pch.h"

#include "helper.h"
#include "image.h"

struct material {
    char name[UINT8_MAX];
    Image baseColor;
    VkDescriptorSet samplerDescriptor;
} typedef Material;

extern const uint32_t textureSizeMaxDimensionLimit;

extern const uint32_t materialCountLimit;
extern uint32_t materialCount;
extern Material *materials;

void loadTexture(const char *path, Image *texture);
void createDescriptor(Material *material);
void loadMaterial(cgltf_material *materialData);
