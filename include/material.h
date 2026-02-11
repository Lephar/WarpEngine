#pragma once

#include "pch.h"

typedef struct image Image;

typedef struct materialUniform {
    vec4 baseColorFactor;
    vec4 metallicRoughnessFactor;
    vec4 emissiveFactor;
    float occlusionScale;
    float normalScale;
} MaterialUniform, *PMaterialUniform;

typedef struct material {
    char name[UINT8_MAX];
    uint32_t isTransparent;
    uint32_t isDoubleSided;
    Image *baseColor;
    Image *metallicRoughness;
    Image *emissive;
    Image *occlusion;
    Image *normal;
    uint32_t factorOffset;
    VkDescriptorSet samplerDescriptorSet;
} Material, *PMaterial;

extern const char *defaultMaterialName;
extern Image *defaultBlackTexture;
extern Image *defaultWhiteTexture;

extern const uint32_t materialTextureCount;
extern uint32_t materialCountLimit;
extern uint32_t materialCount;
extern Material *materials;
extern MaterialUniform *materialUniforms;

void loadDefaultMaterial();
uint32_t findMaterial(const char *name);
void loadMaterial(const char *subdirectory, cgltf_material *materialData);
void bindMaterial(uint32_t framebufferSetIndex, uint32_t framebufferIndex, Material *material);
void destroyMaterial(Material *material);
