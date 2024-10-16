#pragma once

#include "zero.h"

typedef struct memory Memory;

struct image {
    VkImageUsageFlags usage;
    VkDeviceSize size;
    VkImage image;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize offset;
    Memory *memory;
} typedef Image;