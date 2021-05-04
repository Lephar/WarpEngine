#ifndef ZERO_CLIENT_RENDER_SYSTEM_DEVICE_H
#define ZERO_CLIENT_RENDER_SYSTEM_DEVICE_H

#include "renderSystemCore.h"

struct zrDevice {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue queue;
    VkCommandPool commandPool;
};

typedef struct zrDevice zrDevice;

zrDevice createDevice(zrCore);
void destroyDevice(zrDevice);

#endif
