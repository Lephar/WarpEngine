#pragma once

#include "zero.h"

typedef struct memory Memory;

struct image {
    VkDeviceSize offset;
    VkDeviceSize size;
    VkImage image;
    Memory *memory;
} typedef Image;