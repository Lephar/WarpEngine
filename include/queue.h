#pragma once

#include "zero.h"

struct queue {
    VkQueueFlags requiredFlags;
    VkBool32 queueFamilySelected;
    uint32_t queueFamilyIndex;
    uint32_t queueIndex;
    uint32_t queueInfoIndex;
    VkBool32 queueRetrieved;
    VkQueue queue;
    VkBool32 commandStructuresCreated;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
} typedef Queue;

void generateQueueDetails();
void retrieveQueues();
void createCommandStructures();
void destroyCommandStructures();
