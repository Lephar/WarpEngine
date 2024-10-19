#include "buffer.h"
#include "memory.h"

extern VkDevice device;

extern Memory deviceMemory;
extern Memory sharedMemory;

Buffer deviceBuffer;
Buffer sharedBuffer;

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

void createBuffers() {
    createBuffer(&deviceBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, deviceMemory.size / 2);
    bindBufferMemory(&deviceBuffer, &deviceMemory);

    createBuffer(&sharedBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sharedMemory.size);
    bindBufferMemory(&sharedBuffer, &sharedMemory);
}

void destroyBuffer(Buffer *buffer) {
    vkDestroyBuffer(device, buffer->buffer, NULL);

    buffer->memory = NULL;
}

void destroyBuffers() {
    destroyBuffer(&sharedBuffer);
    destroyBuffer(&deviceBuffer);
}
