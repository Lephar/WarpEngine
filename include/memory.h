#pragma once

#include "pch.h"

typedef struct memory {
    VkMemoryPropertyFlags properties;
    uint32_t typeIndex;
    VkDeviceSize size;
    VkDeviceSize offset;
    VkDeviceMemory memory;
} Memory, *PMemory;

extern Memory sharedMemory;
extern Memory contentMemory;
extern Memory frameMemory;

extern void *mappedSharedMemory;

void allocateMemories();
void freeMemories();

VkDeviceSize alignMemory(Memory *memory, VkMemoryRequirements memoryRequirements);
void allocateMemory(Memory *memory, uint32_t typeFilter, VkMemoryPropertyFlags requiredProperties, VkDeviceSize size);
void freeMemory(Memory *memory);
