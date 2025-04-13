#pragma once

#include "pch.h"

typedef struct image Image;

struct descriptorPool {
    uint32_t binding;
    VkDescriptorType type;
    uint32_t count;
    VkShaderStageFlags stage;
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
} typedef DescriptorPool;

extern VkSampler sampler;

extern DescriptorPool sceneDescriptorPool;
extern DescriptorPool primitiveDescriptorPool;
extern DescriptorPool materialDescriptorPool;

void createSampler();

void createDescriptorPool(DescriptorPool *descriptorPool, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);

VkDescriptorSet getSceneDescriptorSet(uint32_t index);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t index);
VkDescriptorSet getMaterialDescriptorSet(Image *image);
