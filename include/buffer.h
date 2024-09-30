#pragma once

#include "zero.h"

typedef struct memory Memory;

struct buffer {
    VkDeviceSize offset;
    VkDeviceSize size;
    VkBuffer buffer;
    Memory *memory;
} typedef Buffer;
