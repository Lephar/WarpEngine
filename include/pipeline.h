#pragma once

#include "pch.h"

extern VkDescriptorSetLayout descriptorSetLayout;
extern VkPipelineLayout pipelineLayout;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

extern VkSampler sampler;

void createPipeline();
void destroyPipeline();
