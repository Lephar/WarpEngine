#pragma once

#include "pch.h"

extern VkDescriptorSetLayout descriptorSetLayout;
extern VkPipelineLayout pipelineLayout;

extern VkDescriptorPool modelDescriptorPool;
extern VkDescriptorPool cameraDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

extern VkSampler sampler;

void createPipeline();
void destroyPipeline();
