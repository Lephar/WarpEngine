#include "buffer.h"

#include "helper.h"
#include "memory.h"
#include "queue.h"

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
    debug("Device local buffer created: %ld bytes", deviceBuffer.size);

    createBuffer(&sharedBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sharedMemory.size);
    bindBufferMemory(&sharedBuffer, &sharedMemory);
    debug("Host visible buffer created: %ld bytes", sharedBuffer.size);
}

void *mapBufferMemory(Buffer *buffer) {
    void *memory;

    vkMapMemory(device, buffer->memory->memory, buffer->memoryOffset, buffer->size, 0, &memory);

    return memory;
}

void copyBuffer(Buffer *source, Buffer *destination, VkDeviceSize sourceOffset, VkDeviceSize destinationOffset, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();

    VkBufferCopy copyInfo = {
        .srcOffset = sourceOffset,
        .dstOffset = destinationOffset,
        .size = size
    };

    vkCmdCopyBuffer(commandBuffer, source->buffer, destination->buffer, 1, &copyInfo);
    endSingleTransferCommand(commandBuffer);
}

// WARN: Risk of unmapping the whole memory used by another image or buffer
void unmapBufferMemory(Buffer *buffer) {
    vkUnmapMemory(device, buffer->memory->memory);
}

void destroyBuffer(Buffer *buffer) {
    vkDestroyBuffer(device, buffer->buffer, NULL);

    buffer->memory = NULL;
}

void destroyBuffers() {
    destroyBuffer(&sharedBuffer);
    debug("Host visible buffer destroyed");

    destroyBuffer(&deviceBuffer);
    debug("Device local buffer destroyed");
}
