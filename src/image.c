#include "image.h"

#include "device.h"
#include "queue.h"
#include "memory.h"
#include "buffer.h"

#include "numerics.h"

void wrapImage(Image *image, VkImage handle, uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageTiling tiling) {
    assert(image);

    image->extent.width = width;
    image->extent.height = height;
    image->mips = mips;
    image->samples = samples;
    image->format = format;
    image->usage = usage;
    image->aspect = aspect;
    image->tiling = tiling;
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    image->image = handle;

    vkGetImageMemoryRequirements(device, image->image, &image->memoryRequirements);
}

Image *createImage(uint32_t width, uint32_t height, uint32_t mips, VkSampleCountFlagBits samples, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkImageTiling tiling) {
    uint32_t queueFamilyIndices[distinctQueueFamilyCount];

    for(uint32_t queueIndex = 0; queueIndex < distinctQueueFamilyCount; queueIndex++) {
        queueFamilyIndices[queueIndex] = queueInfos[queueIndex].queueFamilyIndex;
    }

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = width,
            .height = height,
            .depth = 1
        },
        .mipLevels = mips,
        .arrayLayers = 1,
        .samples = samples,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = distinctQueueFamilyCount,
        .pQueueFamilyIndices = queueFamilyIndices,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
    };

    VkImage handle;
    vkCreateImage(device, &imageInfo, nullptr, &handle);

    Image *image = malloc(sizeof(Image));
    wrapImage(image, handle, width, height, mips, samples, format, usage, aspect, tiling);
    return image;
}

void bindImageMemory(Image *image, Memory *memory) {
    image->memory = memory;

    image->memoryOffset = alignMemory(memory, image->memoryRequirements);

    vkBindImageMemory(device, image->image, memory->memory, image->memoryOffset);
}

void createImageView(Image *image) {
    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
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
            .aspectMask = image->aspect,
            .baseMipLevel = 0,
            .levelCount = image->mips,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCreateImageView(device, &imageViewInfo, nullptr, &image->view);
}

void generateMipmaps(Image *image) {
    VkCommandBuffer commandBuffer = beginSingleGraphicsCommand();

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image->image,
        .subresourceRange = {
            .aspectMask = image->aspect,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    uint32_t mipWidth  = image->extent.width;
    uint32_t mipHeight = image->extent.height;

    for(uint32_t mipLevel = 1; mipLevel < image->mips; mipLevel++) {
        barrier.subresourceRange.baseMipLevel = mipLevel - 1;

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit = {
            .srcSubresource = {
                .aspectMask = image->aspect,
                .mipLevel = mipLevel - 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .srcOffsets = {
                {
                    .x = 0,
                    .y = 0,
                    .z = 0
                },
                {
                    .x = (int32_t) mipWidth,
                    .y = (int32_t) mipHeight,
                    .z = 1
                }
            },
            .dstSubresource = {
                .aspectMask = image->aspect,
                .mipLevel = mipLevel,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .dstOffsets = {
                {
                    .x = 0,
                    .y = 0,
                    .z = 0
                },
                {
                    .x = (int32_t) umax(1, mipWidth  / 2),
                    .y = (int32_t) umax(1, mipHeight / 2),
                    .z = 1
                }
            }
        };

        vkCmdBlitImage(commandBuffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        if(mipWidth  > 1) {
            mipWidth  /= 2;
        }
        if(mipHeight > 1) {
            mipHeight /= 2;
        }
    }

    barrier.subresourceRange.baseMipLevel = image->mips - 1;

    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    endSingleGraphicsCommand(commandBuffer);

    image->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void copyBufferToImage(Buffer *buffer, VkDeviceSize bufferOffset, Image *image, uint32_t mipLevel) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();

    VkBufferImageCopy copyInfo = {
        .bufferOffset = bufferOffset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = image->aspect,
            .mipLevel = mipLevel,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = {
            .width  = umax(1, image->extent.width  >> mipLevel),
            .height = umax(1, image->extent.height >> mipLevel),
            .depth  = 1
        }
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer->buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyInfo);
    endSingleTransferCommand(commandBuffer);
}

void copyImageToBuffer(Image *image, uint32_t mipLevel, Buffer *buffer, VkDeviceSize bufferOffset) {
    VkCommandBuffer commandBuffer = beginSingleTransferCommand();

    VkBufferImageCopy copyInfo = {
        .bufferOffset = bufferOffset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = image->aspect,
            .mipLevel = mipLevel,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
        .imageOffset = {
            .x = 0,
            .y = 0,
            .z = 0
        },
        .imageExtent = {
            .width  = umax(1, image->extent.width  >> mipLevel),
            .height = umax(1, image->extent.height >> mipLevel),
            .depth  = 1
        }
    };

    vkCmdCopyImageToBuffer(commandBuffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->buffer, 1, &copyInfo);
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
        targetAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
        targetStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        targetAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        targetStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else if(layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkImageMemoryBarrier memoryBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
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
            .levelCount = image->mips,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };

    vkCmdPipelineBarrier(*commandBuffer, sourceStage, targetStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

    image->layout = layout;
}

void transitionImageLayout(Image *image, VkImageLayout layout) {
    VkCommandBuffer commandBuffer = beginSingleGraphicsCommand();
    recordTransitionImageLayout(&commandBuffer, image, layout);
    endSingleGraphicsCommand(commandBuffer);
}

void destroyImageView(Image *image) {
    vkDestroyImageView(device, image->view, nullptr);
}

void destroyImage(Image *image) {
    vkDestroyImage(device, image->image, nullptr);

    image->memory = nullptr;

    free(image);
}
