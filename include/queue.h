#pragma once

#include "zero.h"

struct queue {
    VkQueueFlags requiredFlags;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    uint32_t queueInfoIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer; //Is this necessary?
} typedef Queue;

void generateQueueDetails();
void getQueues();
VkCommandBuffer beginTransferCommand();
void endTransferCommand(VkCommandBuffer commandBuffer);
void clearQueues();
