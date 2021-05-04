#ifndef ZERO_CLIENT_RENDER_SYSTEM_BUFFER_H
#define ZERO_CLIENT_RENDER_SYSTEM_BUFFER_H

#include <vulkan/vulkan.h>

struct buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
};

typedef struct buffer Buffer;

void createBuffer(VkPhysicalDevice, VkDevice, VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags,
                  VkBuffer *, VkDeviceMemory *);

void copyBuffer(VkDevice, VkQueue, VkCommandPool, VkBuffer, VkBuffer, VkDeviceSize);

#endif //ZERO_CLIENT_RENDER_SYSTEM_BUFFER_H
