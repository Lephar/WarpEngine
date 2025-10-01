#pragma once

#include "pch.h"

typedef struct memory Memory;
typedef struct buffer Buffer;

typedef struct image {
    VkExtent2D extent;
    uint32_t mips;
    VkSampleCountFlagBits samples;
    VkFormat format;
    VkImageUsageFlags usage;
    VkImageAspectFlags aspect;
    VkImageTiling tiling;
    VkImageLayout layout;
    VkImage image;
    VkImageView view;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize memoryOffset;
    Memory *memory;
} Image, *PImage;

void wrapImage(Image *image, VkImage handle, uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageTiling tiling);
Image *createImage(uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageTiling tiling);
void bindImageMemory(Image *image, Memory *memory);
void createImageView(Image *image);
void generateMipmaps(Image *image);
void copyBufferToImage(Buffer *buffer, VkDeviceSize bufferOffset, Image *image, uint32_t mipLevel);
void copyImageToBuffer(Image *image, uint32_t mipLevel, Buffer *buffer, VkDeviceSize bufferOffset);
void recordTransitionImageLayout(VkCommandBuffer *commandBuffer, Image *image, VkImageLayout layout);
void transitionImageLayout(Image *image, VkImageLayout layout);
void destroyImageView(Image *image);
void destroyImage(Image *image);
