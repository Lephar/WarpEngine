#include "image.h"
#include "memory.h"

extern VkDevice device;

void createImage(Image *image, uint32_t width, uint32_t height, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage) {
    image->size.width = width;
    image->size.height = height;
    image->size.depth = 1;
    image->levels = levels;
    image->samples = samples;
    image->format = format;
    image->usage = usage;

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = image->format,
        .extent = image->size,
        .mipLevels = image->levels,
        .arrayLayers = 1,
        .samples = image->samples,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = image->usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    vkCreateImage(device, &imageInfo, NULL, &image->image);
    vkGetImageMemoryRequirements(device, image->image, &image->memoryRequirements);
}

void bindImageMemory(Image *image, Memory *memory) {
    image->memory = memory;

    image->memoryOffset = alignMemory(memory, image->memoryRequirements);

    vkBindImageMemory(device, image->image, memory->memory, image->memoryOffset);
}

void createImageView(Image *image, VkImageAspectFlags aspects) {
    image->aspects = aspects;

    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = image->image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = image->format,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = image->aspects,
            .baseMipLevel = 0,
            .levelCount = image->levels,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCreateImageView(device, &imageViewInfo, NULL, &image->view);
}

void destroyImageView(Image *image) {
    vkDestroyImageView(device, image->view, NULL);
}

void destroyImage(Image *image) {
    vkDestroyImage(device, image->image, NULL);

    image->memory = NULL;
}