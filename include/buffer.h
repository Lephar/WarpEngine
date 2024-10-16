#pragma once

#include "zero.h"

typedef struct memory Memory;

struct buffer {
    VkBufferUsageFlags usage;
    VkDeviceSize size;
    VkBuffer buffer;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize memoryOffset;
    Memory *memory;
} typedef Buffer;

void createBuffer(Buffer *buffer, VkBufferUsageFlags usage, VkDeviceSize size);
void bindBufferMemory(Buffer *buffer, Memory *memory);
void destroyBuffer(Buffer *buffer);
