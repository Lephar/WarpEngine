#ifndef ZERO_CLIENT_RENDER_SYSTEM_MEMORY_H
#define ZERO_CLIENT_RENDER_SYSTEM_MEMORY_H

#include <vulkan/vulkan.h>

struct zrBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
} typedef zrBuffer;

struct zrImage {
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
} typedef zrImage;

#endif
