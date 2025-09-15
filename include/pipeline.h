#pragma once

#include "pch.h"

extern uint32_t descriptorSetLayoutCount;
extern VkDescriptorSetLayout *descriptorSetLayouts;
extern VkPipelineLayout pipelineLayout;

void setPipelineDetails();
void createPipeline();
void bindPipeline(VkCommandBuffer commandBuffer);
void destroyPipeline();
