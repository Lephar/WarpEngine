#pragma once

#include "zero.h"

typedef struct memory Memory;

struct image {
    VkExtent3D size;
    uint32_t levels;
    VkSampleCountFlagBits samples;
    VkFormat format;
    VkImageUsageFlags usage;
    VkImage image;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize memoryOffset;
    Memory *memory;
} typedef Image;

void createImage(Image *image, VkExtent2D extent, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage);
void bindImageMemory(Image *image, Memory *memory);
void destroyImage(Image *image);
