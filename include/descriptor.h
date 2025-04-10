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

extern VkDescriptorSetLayout descriptorSetLayout;

extern VkSampler sampler;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

void createSampler();
void createDescriptorSetLayout();
void createDescriptorPools();

VkDescriptorSet getSceneDescriptorSet(uint32_t index);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t index);
VkDescriptorSet getMaterialDescriptorSet(Image *image);
