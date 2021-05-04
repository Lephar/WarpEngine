#ifndef ZERO_CLIENT_RENDER_SYSTEM_MEMORY_H
#define ZERO_CLIENT_RENDER_SYSTEM_MEMORY_H

#include <vulkan/vulkan.h>

struct zrBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
};

struct zrImage {
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

typedef struct zrBuffer zrBuffer;
typedef struct zrImage zrImage;

#endif
