#pragma once

#include "pch.h"

typedef struct image Image;

struct material {
    char name[UINT8_MAX];
    uint32_t isTransparent;
    uint32_t isDoubleSided;
    vec4 baseColorFactor;
    Image *baseColor;
    vec2 metallicRoughnessFactor;
    Image *metallicRoughness;
    float normalScale;
    Image *normal;
    vec3 emissiveFactor;
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
