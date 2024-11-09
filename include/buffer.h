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

void createBuffers();
void destroyBuffers();

void createBuffer(Buffer *buffer, VkBufferUsageFlags usage, VkDeviceSize size);
void bindBufferMemory(Buffer *buffer, Memory *memory);
void *mapBufferMemory(Buffer *buffer);
void copyBuffer(Buffer *source, Buffer *destination, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset, VkDeviceSize size);
void unmapBufferMemory(Buffer *buffer);
void destroyBuffer(Buffer *buffer);
