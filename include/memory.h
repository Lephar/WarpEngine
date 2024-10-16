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
void allocateMemory(Memory *memory, uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size);
void allocateMemories();
void freeMemory(Memory *memory);
void freeMemories();
