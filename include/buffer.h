#pragma once

#include "zero.h"

typedef struct memory Memory;

struct buffer {
    VkBool32 bufferCreated;
    VkDeviceSize size;
    VkBuffer buffer;
    VkBool32 memoryBound;
    VkDeviceSize memoryOffset;
    Memory *memory;
} typedef Buffer;
