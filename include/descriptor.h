#pragma once

#include "pch.h"

extern VkDescriptorSetLayout descriptorSetLayout;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

void createDescriptorSetLayout();
void createDescriptorPool(VkDescriptorType type, uint32_t count, VkDescriptorPool *outDescriptorPool);
