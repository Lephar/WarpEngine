#include "buffer.h"
#include "memory.h"

extern VkDevice device;

void createBuffer(Buffer *buffer, VkBufferUsageFlags usage, VkDeviceSize size)
{
    buffer->usage = usage;
    buffer->size = size;

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL
    };

    vkCreateBuffer(device, &bufferInfo, NULL, &buffer->buffer);
    vkGetBufferMemoryRequirements(device, buffer->buffer, &buffer->memoryRequirements);
}

void bindBufferMemory(Buffer *buffer, Memory *memory) {
    buffer->memory = memory;

    buffer->memoryOffset = alignMemory(memory, buffer->memoryRequirements);

    vkBindBufferMemory(device, buffer->buffer, memory->memory, buffer->memoryOffset);
}

void destroyBuffer(Buffer *buffer) {
    vkDestroyBuffer(device, buffer->buffer, NULL);

    buffer->memory = NULL;
}