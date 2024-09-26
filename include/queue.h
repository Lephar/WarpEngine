#pragma once

#include "zero.h"

struct queue {
    uint32_t queueInfoIndex;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
} typedef Queue;

void generateQueueDetails();
void retrieveQueues();
