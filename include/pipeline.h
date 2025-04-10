#pragma once

#include "pch.h"

extern VkPipelineLayout pipelineLayout;

void createPipeline();
void bindPipeline(VkCommandBuffer commandBuffer);
void destroyPipeline();
