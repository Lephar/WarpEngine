#pragma once

#include "pch.h"

typedef struct image Image;

struct material {
    char name[UINT8_MAX];
    bool transparent;
    vec4 baseColorFactor;
    Image *baseColor;
    vec2 metallicRoughnessFactor;
    Image *metallicRoughness;
    Image *normal;
    uint32_t factorOffset;
    VkDescriptorSet materialDescriptorSet;
} typedef Material;

extern const uint32_t materialTextureCount;

extern Image *defaultBlackTexture;
extern Image *defaultWhiteTexture;

extern VkDescriptorSet factorDescriptorSet;

extern uint32_t materialCount;
extern Material *materials;

uint32_t findMaterial(cgltf_material *materialData);
void loadMaterial(const char *subdirectory, Material *material, cgltf_material *materialData);
void bindMaterial(VkCommandBuffer commandBuffer, Material *material);
void destroyMaterial(Material *material);
