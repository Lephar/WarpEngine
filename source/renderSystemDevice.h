#ifndef ZERO_CLIENT_RENDER_SYSTEM_DEVICE_H
#define ZERO_CLIENT_RENDER_SYSTEM_DEVICE_H

#include "renderSystemCore.h"

struct device {
    VkDevice device;
    uint32_t queueFamilyIndex;
    VkPhysicalDevice physicalDevice;
    VkQueue queue;
    VkCommandPool commandPool;
};

typedef struct device Device;

Device createDevice(Core);

VkCommandBuffer beginSingleTimeCommand(VkDevice, VkCommandPool);

void endSingleTimeCommand(VkDevice, VkQueue, VkCommandPool, VkCommandBuffer);

void destroyDevice(Device);

#endif
