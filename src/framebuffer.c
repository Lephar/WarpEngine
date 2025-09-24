#include "framebuffer.h"

#include "surface.h"
#include "physicalDevice.h"
#include "device.h"
#include "queue.h"
#include "memory.h"
#include "image.h"
#include "descriptor.h"

#include "logger.h"

const uint32_t framebufferSetCountLimit = 3;
const uint32_t framebufferSetFramebufferCountLimit = 3;

uint32_t framebufferSetCount;
FramebufferSet *framebufferSets;

void createFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    framebuffer->resolve = createImage(framebufferSet->extent.width, framebufferSet->extent.height, 1, VK_SAMPLE_COUNT_1_BIT, framebufferSet->colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);

    bindImageMemory(framebuffer->resolve, &frameMemory);
    createImageView(framebuffer->resolve);

    transitionImageLayout(framebuffer->resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    framebuffer->cameraDescriptorSet    = getCameraDescriptorSet(   framebufferSetIndex, framebufferIndex);
    framebuffer->primitiveDescriptorSet = getPrimitiveDescriptorSet(framebufferSetIndex, framebufferIndex);
    framebuffer->materialDescriptorSet  = getMaterialDescriptorSet( framebufferSetIndex, framebufferIndex);

    framebuffer->renderCommandBuffer  = allocateSingleCommandBuffer(&graphicsQueue);
    framebuffer->presentCommandBuffer = allocateSingleCommandBuffer(&graphicsQueue);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    vkCreateSemaphore(device, &semaphoreInfo, nullptr, &framebuffer->drawSemaphore);
    vkCreateSemaphore(device, &semaphoreInfo, nullptr, &framebuffer->blitSemaphore);

    VkFenceCreateInfo unsignaledFenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0
    };

    VkFence semaphoreSignalFence;
    vkCreateFence(device, &unsignaledFenceInfo, nullptr, &semaphoreSignalFence);

    // TODO: This is not an elegant solution, change to timeline semaphores perhaps?
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 0,
        .pCommandBuffers = nullptr,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->blitSemaphore
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, semaphoreSignalFence);
    vkWaitForFences(device, 1, &semaphoreSignalFence, true, UINT64_MAX);
    vkDestroyFence(device, semaphoreSignalFence, nullptr);

    VkFenceCreateInfo signaledFenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    vkCreateFence(device, &signaledFenceInfo, nullptr, &framebuffer->drawFence);
    vkCreateFence(device, &signaledFenceInfo, nullptr, &framebuffer->blitFence);
}

void createFramebufferSet(uint32_t framebufferSetIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];

    framebufferSet->extent = surfaceExtent;
    framebufferSet->framebufferCount = 2;
    framebufferSet->sampleCount = VK_SAMPLE_COUNT_4_BIT;

    assert(framebufferSet->sampleCount & physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    assert(framebufferSet->sampleCount & physicalDeviceProperties.limits.framebufferColorSampleCounts);

    framebufferSet->depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    framebufferSet->colorFormat        = VK_FORMAT_R8G8B8A8_SRGB;

    framebufferSet->depthStencil = createImage(framebufferSet->extent.width, framebufferSet->extent.height, 1, framebufferSet->sampleCount, framebufferSet->depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_TILING_OPTIMAL);
    framebufferSet->color        = createImage(framebufferSet->extent.width, framebufferSet->extent.height, 1, framebufferSet->sampleCount, framebufferSet->colorFormat,        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,         VK_IMAGE_ASPECT_COLOR_BIT,                               VK_IMAGE_TILING_OPTIMAL);

    bindImageMemory(framebufferSet->depthStencil, &frameMemory);
    bindImageMemory(framebufferSet->color,        &frameMemory);

    createImageView(framebufferSet->depthStencil);
    createImageView(framebufferSet->color);

    transitionImageLayout(framebufferSet->depthStencil, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    transitionImageLayout(framebufferSet->color,        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    framebufferSet->framebuffers = malloc(framebufferSet->framebufferCount * sizeof(Framebuffer));

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet->framebufferCount; framebufferIndex++) {
        debug("Framebuffer %d:", framebufferIndex);
        createFramebuffer(framebufferSetIndex, framebufferIndex);
        debug("\tSuccessfully created");
    }
}

void createFramebufferSets() {
    framebufferSetCount = 3; // TODO: This is arbitrary
    framebufferSets = malloc(framebufferSetCount * sizeof(FramebufferSet));

    for(uint32_t framebufferSetIndex = 0; framebufferSetIndex < framebufferSetCount; framebufferSetIndex++) {
        createFramebufferSet(framebufferSetIndex);
    }
}

void waitFramebufferDraw(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkWaitForFences(device, 1, &framebuffer->drawFence, VK_TRUE, UINT64_MAX);
    vkResetFences(  device, 1, &framebuffer->drawFence);
}

void waitFramebufferBlit(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkWaitForFences(device, 1, &framebuffer->blitFence, VK_TRUE, UINT64_MAX);
    vkResetFences(  device, 1, &framebuffer->blitFence);
}

void beginFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };

    VkRenderingAttachmentInfo depthStencilAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = framebufferSet->depthStencil->view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = nullptr,
        .resolveImageLayout = 0,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue = {
            .depthStencil = {
                .depth = 1.0f,
                .stencil = 0
            }
        }
    };

    VkRenderingAttachmentInfo colorAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = framebufferSet->color->view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
        .resolveImageView = framebuffer->resolve->view,
        .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {
            .color = {
                .float32 = {
                    0.0f,
                    0.0f,
                    0.0f,
                    0.0f
                },
            }
        }
    };

    VkRenderingInfo renderingInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = {
            .offset = {
                .x = 0,
                .y = 0,
            },
            .extent = framebufferSet->extent,
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthStencilAttachmentInfo,
        .pStencilAttachment = &depthStencilAttachmentInfo
    };

    vkBeginCommandBuffer(framebuffer->renderCommandBuffer, &beginInfo);
    vkCmdBeginRendering(framebuffer->renderCommandBuffer, &renderingInfo);
}

void bindFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    VkSampleMask sampleMask = 0xFF;

    VkViewport viewport = {
        .x = 0.0f,
        .y = (float) framebufferSet->extent.height,
        .width = (float) framebufferSet->extent.width,
        .height = - (float) framebufferSet->extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {
            .x = 0,
            .y = 0
        },
        .extent = framebufferSet->extent
    };

    PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadDeviceFunction("vkCmdSetRasterizationSamplesEXT");
    cmdSetRasterizationSamples(framebuffer->renderCommandBuffer, framebufferSet->sampleCount);
    PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadDeviceFunction("vkCmdSetSampleMaskEXT");
    cmdSetSampleMask(framebuffer->renderCommandBuffer, framebufferSet->sampleCount, &sampleMask);

    vkCmdSetViewportWithCount(framebuffer->renderCommandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount( framebuffer->renderCommandBuffer, 1, &scissor);
}

void endFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkCmdEndRendering( framebuffer->renderCommandBuffer);
    vkEndCommandBuffer(framebuffer->renderCommandBuffer);
}

void destroyFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkDestroyFence(device, framebuffer->blitFence, nullptr);
    vkDestroyFence(device, framebuffer->drawFence, nullptr);

    vkDestroySemaphore(device, framebuffer->blitSemaphore, nullptr);
    vkDestroySemaphore(device, framebuffer->drawSemaphore, nullptr);

    destroyImageView(framebuffer->resolve);
    destroyImage(    framebuffer->resolve);
}

void destroyFramebufferSet(uint32_t framebufferSetIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet->framebufferCount; framebufferIndex++) {
        destroyFramebuffer(framebufferSetIndex, framebufferIndex);
        debug("Framebuffer %d destroyed", framebufferIndex);
    }

    destroyImageView(framebufferSet->color);
    destroyImageView(framebufferSet->depthStencil);

    destroyImage(framebufferSet->color);
    destroyImage(framebufferSet->depthStencil);

    resetDescriptorPool(&materialDescriptorPool);
    resetDescriptorPool(&primitiveDescriptorPool);
    resetDescriptorPool(&cameraDescriptorPool);

    free(framebufferSet->framebuffers);
}

void destroyFramebufferSets() {
    for(uint32_t framebufferSetIndex = 0; framebufferSetIndex < framebufferSetCount; framebufferSetIndex++) {
        destroyFramebufferSet(framebufferSetIndex);
    }

    free(framebufferSets);
    framebufferSetCount = 0;

    frameMemory.offset = 0;
}