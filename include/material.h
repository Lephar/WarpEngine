#pragma once

#include "pch.h"

typedef struct image Image;

typedef struct materialUniform {
    vec4 baseColorFactor;
    vec2 metallicRoughnessFactor;
    vec3 emissiveFactor;
    float normalScale;
} MaterialUniform, *PMaterialUniform;

typedef struct material {
    char name[UINT8_MAX];
    uint32_t isTransparent;
    uint32_t isDoubleSided;
    Image *baseColor;
    Image *metallicRoughness;
    Image *normal;
    uint32_t factorOffset;
    VkDescriptorSet materialDescriptorSet;
} Material, *PMaterial;

extern Image *defaultBlackTexture;
extern Image *defaultWhiteTexture;

extern const uint32_t materialTextureCount;
extern uint32_t materialCountLimit;
extern uint32_t materialCount;
extern Material *materials;
extern MaterialUniform *materialUniforms;

extern VkDescriptorSet factorDescriptorSet;

uint32_t findMaterial(cgltf_material *materialData);
void loadMaterial(const char *subdirectory, cgltf_material *materialData);
void bindMaterial(VkCommandBuffer commandBuffer, Material *material);
void destroyMaterial(Material *material);
