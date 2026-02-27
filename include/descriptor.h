#pragma once

#include "pch.h"

typedef struct material Material;

typedef struct descriptorBinding {
    uint32_t bindingIndex;
    union {
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorImageInfo imageInfo;
    };
} DescriptorBinding, *PDescriptorBinding;

typedef struct descriptorPool {
    VkDescriptorType type;
    VkShaderStageFlags stage;
    uint32_t setIndex;
    uint32_t setCount;
    uint32_t bindingCount;
    PDescriptorBinding bindings;
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
} DescriptorPool, *PDescriptorPool;

extern DescriptorPool   storageDescriptorPool;
extern DescriptorPool primitiveDescriptorPool;
extern DescriptorPool    cameraDescriptorPool;
extern DescriptorPool  lightingDescriptorPool;
extern DescriptorPool  materialDescriptorPool;
extern DescriptorPool   samplerDescriptorPool;

extern const PDescriptorPool descriptorPoolReferences[];
extern const uint32_t        descriptorPoolCount;

extern VkSampler sampler;
extern VkDescriptorSet storageDescriptorSet;

void createSampler();

void createDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags stage, uint32_t bindingCount);

VkDescriptorSet getStorageDescriptorSet(VkDeviceSize offsets[], VkDeviceSize ranges[]);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getCameraDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getMaterialDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
VkDescriptorSet getSamplerDescriptorSet(Material *material);
VkDescriptorSet getLightingDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex);

void resetDescriptorPool(DescriptorPool *descriptorPool);
void destroyDescriptorPool(DescriptorPool *descriptorPool);
