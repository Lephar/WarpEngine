#pragma once

#include "pch.h"

typedef struct image Image;

extern VkDescriptorSetLayout descriptorSetLayout;

extern VkSampler sampler;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

void createDescriptorSetLayout();
void createSampler();

VkDescriptorPool createDescriptorPool(VkDescriptorType type, uint32_t count);

VkDescriptorSet getSceneDescriptorSet(uint32_t index);
VkDescriptorSet getPrimitiveDescriptorSet(uint32_t index);
VkDescriptorSet getMaterialDescriptorSet(Image *image);
