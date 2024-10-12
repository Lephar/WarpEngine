#pragma once

#include "zero.h"

struct memory {
    VkBool32 allocated;
    VkMemoryPropertyFlags requiredProperties;
    uint32_t typeIndex;
    VkDeviceSize size;
    VkDeviceSize offset;
    VkDeviceMemory memory;
} typedef Memory;

Memory allocateMemory(uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size);
VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements);
void generateMemoryDetails();
void allocateMemories();
void freeMemories();
