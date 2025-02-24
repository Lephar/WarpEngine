#pragma once

#include "pch.h"

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
void copyBuffer(Buffer *source, Buffer *destination, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset, VkDeviceSize size);
void stagingBufferCopy(void *sourceBuffer, uint64_t sourceOffset, VkDeviceSize destinationOffset, uint64_t size);
void destroyBuffer(Buffer *buffer);
