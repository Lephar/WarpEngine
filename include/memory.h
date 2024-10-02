#pragma once

#include "zero.h"

struct memory {
    uint32_t type;
    VkDeviceMemory memory;
    VkDeviceSize size;
    VkDeviceSize offset;
} typedef Memory;

uint32_t chooseMemoryType(uint32_t filter, VkMemoryPropertyFlags flags);

void generateMemoryDetails();
void allocateMemories();
void freeMemories();