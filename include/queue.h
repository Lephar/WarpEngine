#pragma once

#include "zero.h"

struct queue {
    VkQueueFlags requiredFlags;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    uint32_t queueInfoIndex;
    VkQueue queue;
    VkCommandPool commandPool;
} typedef Queue;

void generateQueueDetails();
void getQueues();
VkCommandBuffer allocateSingleCommandBuffer(Queue *queue);
VkCommandBuffer beginSingleTransferCommand();
VkCommandBuffer beginSingleGraphicsCommand();
void endSingleTransferCommand(VkCommandBuffer commandBuffer);
void endSingleGraphicsCommand(VkCommandBuffer commandBuffer);
void clearQueues();
