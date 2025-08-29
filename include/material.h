#pragma once

#include "pch.h"

typedef struct image Image;

struct material {
    char name[UINT8_MAX];
    bool transparent;
    vec4 baseColorFactor;
    Image *baseColor;
    float metallicFactor;
    float roughnessFactor;
    Image *metallicRoughness;
    Image *normal;
    VkDescriptorSet descriptorSet;
} typedef Material;

extern Image *defaultBlackTexture;
extern Image *defaultWhiteTexture;

extern uint32_t materialCount;
extern Material *materials;

uint32_t findMaterial(cgltf_material *materialData);
void loadMaterial(const char *subdirectory, Material *material, cgltf_material *materialData);
void bindMaterial(VkCommandBuffer commandBuffer, Material *material);
void destroyMaterial(Material *material);
