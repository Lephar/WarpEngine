#include "image.h"
#include "memory.h"

extern VkDevice device;

void createImage(uint32_t imageWidth, uint32_t imageHeight, uint32_t levels, VkSampleCountFlagBits samples,
 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
 VkImage *image, VkDeviceMemory *memory)
{
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = imageWidth,
            .height = imageHeight,
            .depth = 1
        },
        .mipLevels = levels,
        .arrayLayers = 1,
        .samples = samples,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    vkCreateImage(device, &imageInfo, NULL, image);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, *image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    //allocateInfo.memoryTypeIndex = chooseMemoryType(memoryRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device, &allocateInfo, NULL, memory);
    vkBindImageMemory(device, *image, *memory, 0);
}