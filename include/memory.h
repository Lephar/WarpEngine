#pragma once

#include "zero.h"

struct memory {
    VkMemoryPropertyFlags requiredProperties;
    uint32_t typeIndex;
    VkDeviceSize size;
    VkDeviceSize offset;
    VkDeviceMemory memory;
} typedef Memory;

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements);
void allocateMemory(Memory *memory, VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags requiredProperties);
void allocateMemories();
void freeMemory(Memory *memory);
void freeMemories();
