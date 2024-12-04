#pragma once

#include "zero.h"

typedef struct memory Memory;
typedef struct buffer Buffer;

struct image {
    VkExtent3D extent;
    uint32_t levels;
    VkSampleCountFlagBits samples;
    VkFormat format;
    VkImageUsageFlags usage;
    VkImageAspectFlags aspects;
    VkImage image;
    VkImageView view;
    VkMemoryRequirements memoryRequirements;
    VkDeviceSize memoryOffset;
    Memory *memory;
} typedef Image;

void createImage(Image *image, uint32_t width, uint32_t height, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage);
void createImageView(Image *image, VkImageAspectFlags aspects);
void bindImageMemory(Image *image, Memory *memory);
void copyBufferToImage(Buffer *buffer, Image *image, VkDeviceSize bufferOffset);
void recordTransitionImageLayout(VkCommandBuffer *commandBuffer, Image *image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags sourceAccessFlags, VkAccessFlags destinationAccessFlags, VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages);
void transitionImageLayout(Image *image, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags sourceAccessFlags, VkAccessFlags destinationAccessFlags, VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages);
void destroyImageView(Image *image);
void destroyImage(Image *image);
