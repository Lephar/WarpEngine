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
void createBuffers();
void bindBufferMemory(Buffer *buffer, Memory *memory);
void copyBuffer(Buffer *source, Buffer *destination, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset, VkDeviceSize size);
void destroyBuffer(Buffer *buffer);
void destroyBuffers();
