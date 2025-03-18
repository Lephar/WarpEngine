#pragma once

#include "pch.h"

struct memory {
    VkMemoryPropertyFlags properties;
    uint32_t typeIndex;
    VkDeviceSize size;
    VkDeviceSize reusableMemoryOffset;
    VkDeviceSize offset;
    VkDeviceMemory memory;
} typedef Memory;

extern Memory deviceMemory;
extern Memory sharedMemory;

extern void *mappedSharedMemory;

void allocateMemories();
void freeMemories();

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements);
void allocateMemory(Memory *memory, uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size);
void freeMemory(Memory *memory);
