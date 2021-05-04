#include "renderSystemImage.h"
#include "renderSystemDevice.h"

void createImage(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t imageWidth, uint32_t imageHeight,
                 uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageTiling tiling,
                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image,
                 VkDeviceMemory *imageMemory) {
    VkImageCreateInfo imageInfo = {
            .imageType = VK_IMAGE_TYPE_2D,
            .extent.width = imageWidth,
            .extent.height = imageHeight,
            .extent.depth = 1,
            .mipLevels = mipLevels,
            .arrayLayers = 1,
            .format = format,
            .tiling = tiling,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .usage = usage,
            .samples = samples,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateImage(device, &imageInfo, NULL, image);

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(device, *image, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = chooseMemoryType(physicalDevice, memoryRequirements.memoryTypeBits, properties)
    };

    vkAllocateMemory(device, &allocateInfo, NULL, imageMemory);
    vkBindImageMemory(device, *image, *imageMemory, 0);
}

void createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags flags, uint32_t mipLevels,
                     VkImageView *view) {
    VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .image = image,
            .format = format,
            .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = {
                    .aspectMask = flags,
                    .levelCount = mipLevels,
                    .baseMipLevel = 0,
                    .layerCount = 1,
                    .baseArrayLayer = 0
            }
    };

    vkCreateImageView(device, &viewInfo, NULL, view);
}

void copyBufferToImage(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkBuffer buffer, VkImage image,
                       uint32_t imageWidth, uint32_t imageHeight) {
    VkBufferImageCopy copyInfo = {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            },
            .imageOffset = {
                    .x = 0,
                    .y = 0,
                    .z = 0
            },
            .imageExtent = {
                    .width = imageWidth,
                    .height = imageHeight,
                    .depth = 1
            }
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}

void transitionImageLayout(VkDevice device, VkQueue queue, VkCommandPool commandPool, VkImage image,
                           VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    VkAccessFlags sourceAccess = 0, destinationAccess = 0;
    VkPipelineStageFlags sourceStage = 0, destinationStage = 0;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        sourceAccess = 0;
        destinationAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        sourceAccess = VK_ACCESS_TRANSFER_WRITE_BIT;
        destinationAccess = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    VkImageMemoryBarrier barrier = {
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcAccessMask = sourceAccess,
            .dstAccessMask = destinationAccess,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount = 1
            }
    };

    VkCommandBuffer commandBuffer = beginSingleTimeCommand(device, commandPool);
    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    endSingleTimeCommand(device, queue, commandPool, commandBuffer);
}
