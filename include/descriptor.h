#pragma once

#include "pch.h"

typedef struct material Material;

struct descriptorPool {
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

void createBufferDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);
void createSamplerDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);

VkDescriptorSet getSceneDescriptorSet(uint32_t framebufferIndex);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferIndex);
VkDescriptorSet getFactorDescriptorSet();
VkDescriptorSet getMaterialDescriptorSet(Material *material);

void resetDescriptorPool(DescriptorPool *descriptorPool);
void destroyDescriptorPool(DescriptorPool *descriptorPool);
