#pragma once

#include "pch.h"

typedef struct memory Memory;
typedef struct buffer Buffer;

struct image {
    VkExtent3D extent;
    uint32_t mips;
    VkSampleCountFlagBits samples;
    VkFormat format;
    VkImageUsageFlags usage;
    VkImageAspectFlags aspect;
    VkImageLayout layout;
    VkImage image;
    VkImageView view;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize memoryOffset;
    Memory *memory;
} typedef Image;

void wrapImage(Image *image, VkImage handle, uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect);
Image *createImage(uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect);
void bindImageMemory(Image *image, Memory *memory);
void createImageView(Image *image);
void generateMipmaps(Image *image);
void copyBufferToImage(Buffer *buffer, VkDeviceSize bufferOffset, Image *image, uint32_t mipLevel);
void recordTransitionImageLayout(VkCommandBuffer *commandBuffer, Image *image, VkImageLayout layout);
void transitionImageLayout(Image *image, VkImageLayout layout);
void destroyImageView(Image *image);
void destroyImage(Image *image);
