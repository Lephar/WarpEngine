#pragma once

#include "pch.h"

typedef struct material Material;

typedef struct descriptorPool {
    VkDescriptorType type;
    uint32_t count;
    VkShaderStageFlags stage;
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
} DescriptorPool, *PDescriptorPool;

extern VkSampler sampler;

extern DescriptorPool cameraDescriptorPool;
extern DescriptorPool primitiveDescriptorPool;
extern DescriptorPool materialDescriptorPool;
extern DescriptorPool samplerDescriptorPool;

void createSampler();

void createBufferDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);
void createSamplerDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage);

VkDescriptorSet getCameraDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getMaterialDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getSamplerDescriptorSet(Material *material);

void resetDescriptorPool(DescriptorPool *descriptorPool);
void destroyDescriptorPool(DescriptorPool *descriptorPool);
