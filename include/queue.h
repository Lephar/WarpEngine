#pragma once

#include "pch.h"

struct queue {
    VkQueueFlags requiredFlags;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    uint32_t queueInfoIndex;
    VkQueue queue;
    VkCommandPool commandPool;
} typedef Queue;

extern uint32_t distinctQueueFamilyCount;
extern VkDeviceQueueCreateInfo *queueInfos;

extern Queue graphicsQueue;
extern Queue  computeQueue;
extern Queue transferQueue;

extern uint32_t queueCount;
extern Queue *queueReferences[];

void generateQueueDetails();
void getQueues();
VkCommandBuffer allocateSingleCommandBuffer(Queue *queue);
VkCommandBuffer beginSingleTransferCommand();
VkCommandBuffer beginSingleGraphicsCommand();
void endSingleTransferCommand(VkCommandBuffer commandBuffer);
void endSingleGraphicsCommand(VkCommandBuffer commandBuffer);
void clearQueues();
