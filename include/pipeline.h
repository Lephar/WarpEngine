#pragma once

#include "pch.h"

extern VkSampler sampler;

extern VkDescriptorPool descriptorPool;
extern VkDescriptorSetLayout descriptorSetLayout;

extern VkPipelineLayout pipelineLayout;

void createSampler();
void createLayouts();
void createDescriptorPool();
void destroyPipeline();
