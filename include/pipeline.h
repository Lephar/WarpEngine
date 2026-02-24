#pragma once

#include "pch.h"

extern uint32_t descriptorSetLayoutCount;
extern VkDescriptorSetLayout *descriptorSetLayouts;

extern uint32_t pushConstantRangeCount;
extern VkPushConstantRange *pushConstantRanges;

extern VkPipelineLayout pipelineLayout;

void setPipelineDetails();
void createPipeline();
void bindPipeline(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void destroyPipeline();
