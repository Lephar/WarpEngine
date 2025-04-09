#pragma once

#include "pch.h"

typedef struct buffer Buffer;
typedef struct image  Image;

struct sceneUniform {
    mat4 anchor;
    mat4 camera;
} typedef SceneUniform;

struct primitiveUniform {
    mat4 transform;
} typedef PrimitiveUniform;

extern VkSampler sampler;

extern VkDescriptorSetLayout descriptorSetLayout;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

void createDescriptorSetLayout();
void createDescriptorPools();

VkDescriptorSet getSceneDescriptorSet(uint32_t frameIndex);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t frameIndex);
VkDescriptorSet getMaterialDescriptorSet(Image *image);
