#include "renderSystemBuffer.h"
#include "renderSystemDevice.h"

uint32_t chooseMemoryType(VkPhysicalDevice physicalDevice, uint32_t filter, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t index = 0u; index < memoryProperties.memoryTypeCount; index++)
        if ((filter & (1 << index)) && (memoryProperties.memoryTypes[index].propertyFlags & flags) == flags)
            return index;

    return UINT32_MAX;
}

void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
    VkBufferCreateInfo bufferInfo = {
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = usage,
            .size = size
    };

    vkCreateBuffer(device, &bufferInfo, NULL, buffer);

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, *buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = chooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    vkAllocateMemory(device, &allocateInfo, NULL, bufferMemory);
    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
}

void copyBuffer(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer,
                VkDeviceSize size) {
    VkBufferCopy copyRegion = {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}