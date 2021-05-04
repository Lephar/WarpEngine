#ifndef ZERO_CLIENT_RENDER_SYSTEM_IMAGE_H
#define ZERO_CLIENT_RENDER_SYSTEM_IMAGE_H

#include "renderSystemBuffer.h"

struct image {
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

typedef struct image Image;

void createImage(VkPhysicalDevice, VkDevice, uint32_t, uint32_t, uint32_t, VkSampleCountFlagBits, VkFormat,
                 VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage *, VkDeviceMemory *);

void createImageView(VkDevice, VkImage, VkFormat, VkImageAspectFlags, uint32_t, VkImageView *);

void copyBufferToImage(VkDevice, VkQueue, VkCommandPool, VkBuffer, VkImage, uint32_t, uint32_t);

void transitionImageLayout(VkDevice, VkQueue, VkCommandPool, VkImage, VkImageLayout, VkImageLayout, uint32_t);

#endif //ZERO_CLIENT_RENDER_SYSTEM_IMAGE_H
