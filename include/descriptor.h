#pragma once

#include "pch.h"

typedef struct material Material;

typedef struct descriptorPool {
    VkDescriptorType type;
    uint32_t descriptorCount;
    VkShaderStageFlags stage;
    uint32_t bindingCount;
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
} DescriptorPool, *PDescriptorPool;

extern VkSampler sampler;

extern DescriptorPool lightingDescriptorPool;
extern DescriptorPool cameraDescriptorPool;
extern DescriptorPool primitiveDescriptorPool;
extern DescriptorPool materialDescriptorPool;
extern DescriptorPool samplerDescriptorPool;
extern DescriptorPool storageDescriptorPool;

extern VkDescriptorSet storageDescriptorSet;

void createSampler();

void createDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags stage, uint32_t bindingCount);

VkDescriptorSet getLightingDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getCameraDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getMaterialDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getSamplerDescriptorSet(Material *material);
VkDescriptorSet getStorageDescriptorSet(VkDeviceSize offsets[], VkDeviceSize ranges[]);

void resetDescriptorPool(DescriptorPool *descriptorPool);
void destroyDescriptorPool(DescriptorPool *descriptorPool);
