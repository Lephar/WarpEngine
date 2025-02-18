#include "image.h"

#include "memory.h"
#include "buffer.h"
#include "queue.h"

extern VkDevice device;

void wrapImage(Image *image, VkImage handle, uint32_t width, uint32_t height, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage) {
    image->extent.width = width;
    image->extent.height = height;
    image->extent.depth = 1;
    image->mips = levels;
    image->samples = samples;
    image->format = format;
    image->usage = usage;
    image->aspects = VK_IMAGE_ASPECT_NONE;
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    image->image = handle;

    vkGetImageMemoryRequirements(device, image->image, &image->memoryRequirements);
}

// TODO: Rename levels, add aspects maybe?
void createImage(Image *image, uint32_t width, uint32_t height, uint32_t levels, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage) {
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1
        },
        .mipLevels = levels,
        .arrayLayers = 1,
        .samples = samples,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkImage imageHandle;
    vkCreateImage(device, &imageInfo, NULL, &imageHandle);

    wrapImage(image, imageHandle, width, height, levels, samples, format, usage);
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
            .levelCount = image->mips,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCreateImageView(device, &imageViewInfo, NULL, &image->view);
}

void copyBufferToImage(Buffer *buffer, Image *image, VkDeviceSize bufferOffset) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();

    VkBufferImageCopy copyInfo = {
        .bufferOffset = bufferOffset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = { // Should image parameters be used?
            .aspectMask = image->aspects,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = image->extent
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer->buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
    endSingleTransferCommand(commandBuffer);
}

void recordTransitionImageLayout(VkCommandBuffer *commandBuffer, Image *image, VkImageLayout layout) {
    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkPipelineStageFlags targetStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkAccessFlags sourceAccessMask = VK_ACCESS_NONE;
    VkAccessFlags targetAccessMask = VK_ACCESS_NONE;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if(image->layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        sourceAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    } else if(image->layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        sourceAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    } else if(image->layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        sourceAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    } else if(image->layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }

    if(layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        targetStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        targetAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        targetStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        targetAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        targetStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        targetAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        targetStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageMemoryBarrier memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = sourceAccessMask,
        .dstAccessMask = targetAccessMask,
        .oldLayout = image->layout,
        .newLayout = layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->image,
        .subresourceRange = {
            .aspectMask = aspectMask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(*commandBuffer, sourceStage, targetStage, 0, 0, NULL, 0, NULL, 1, &memoryBarrier);

    image->layout = layout;
}

void transitionImageLayout(Image *image, VkImageLayout layout) {
    VkCommandBuffer commandBuffer = beginSingleGraphicsCommand();
    recordTransitionImageLayout(&commandBuffer, image, layout);
    endSingleGraphicsCommand(commandBuffer);
}

void destroyImageView(Image *image) {
    vkDestroyImageView(device, image->view, NULL);
}

void destroyImage(Image *image) {
    vkDestroyImage(device, image->image, NULL);

    image->memory = NULL;
}
