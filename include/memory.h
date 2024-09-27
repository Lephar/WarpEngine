#pragma once

#include "zero.h"

struct memory {
    uint32_t type;
    VkDeviceMemory memory;
    VkDeviceSize size;
    VkDeviceSize offset;
} typedef Memory;

void allocateMemories();
void freeMemories();