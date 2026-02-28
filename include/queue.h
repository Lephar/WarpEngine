#pragma once

#include "pch.h"

typedef struct queue {
    VkQueueFlags requiredFlags;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    uint32_t queueInfoIndex;
    VkQueue queue;
    VkCommandPool commandPool;
} Queue, *PQueue;

extern uint32_t distinctQueueFamilyCount;
extern VkDeviceQueueCreateInfo *queueInfos;

extern Queue graphicsQueue;
extern Queue  computeQueue;
extern Queue transferQueue;

extern const PQueue   queueReferences[];
extern const uint32_t queueCount;

void generateQueueDetails();
void getQueues();
VkCommandBuffer allocateSingleCommandBuffer(Queue *queue);
VkCommandBuffer beginSingleTransferCommand();
VkCommandBuffer beginSingleGraphicsCommand();
void endSingleTransferCommand(VkCommandBuffer commandBuffer);
void endSingleGraphicsCommand(VkCommandBuffer commandBuffer);
void clearQueues();
