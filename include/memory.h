#pragma once

#include "zero.h"

struct memory {
    VkBool32 allocated;
    uint32_t typeIndex;
    VkDeviceSize size;
    VkDeviceSize offset;
    VkDeviceMemory memory;
} typedef Memory;

uint32_t chooseMemoryType(uint32_t filter, VkMemoryPropertyFlags flags);

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements);
void generateMemoryDetails();
void allocateMemories();
void freeMemories();