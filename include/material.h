#pragma once

#include "pch.h"

typedef struct image Image;

struct material {
    char name[UINT8_MAX];
    Image *baseColor;
    VkDescriptorSet descriptorSet;
} typedef Material;

extern uint32_t materialCount;
extern Material *materials;

uint32_t findMaterial(cgltf_material *materialData);
void loadMaterial(Material *material, cgltf_material *materialData);
void bindMaterial(VkCommandBuffer commandBuffer, Material *material);
void destroyMaterial(Material *material);
