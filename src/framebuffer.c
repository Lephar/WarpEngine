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

FramebufferSet oldFramebufferSet;
FramebufferSet framebufferSet;

void createFramebuffer(Framebuffer *framebuffer, uint32_t index) {
    framebuffer->resolve = createImage(surfaceExtent.width, surfaceExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, framebufferSet.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);

    bindImageMemory(framebuffer->resolve, &frameMemory);
    createImageView(framebuffer->resolve);

    transitionImageLayout(framebuffer->resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    framebuffer->sceneDescriptorSet     =     getSceneDescriptorSet(index);
    framebuffer->primitiveDescriptorSet = getPrimitiveDescriptorSet(index);

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

void createFramebufferSet() {
    framebufferSet.imageCount = 2;

    framebufferSet.sampleCount = VK_SAMPLE_COUNT_4_BIT;
    assert(framebufferSet.sampleCount & physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    assert(framebufferSet.sampleCount & physicalDeviceProperties.limits.framebufferColorSampleCounts);

    framebufferSet.depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    framebufferSet.colorFormat        = VK_FORMAT_R8G8B8A8_SRGB;

    framebufferSet.depthStencil = createImage(surfaceExtent.width, surfaceExtent.height, 1, framebufferSet.sampleCount, framebufferSet.depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, VK_IMAGE_TILING_OPTIMAL);
    framebufferSet.color        = createImage(surfaceExtent.width, surfaceExtent.height, 1, framebufferSet.sampleCount, framebufferSet.colorFormat,        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,         VK_IMAGE_ASPECT_COLOR_BIT,                               VK_IMAGE_TILING_OPTIMAL);

    bindImageMemory(framebufferSet.depthStencil, &frameMemory);
    bindImageMemory(framebufferSet.color,        &frameMemory);

    createImageView(framebufferSet.depthStencil);
    createImageView(framebufferSet.color);

    transitionImageLayout(framebufferSet.depthStencil, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    transitionImageLayout(framebufferSet.color,        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    framebufferSet.framebuffers = malloc(framebufferSet.imageCount * sizeof(Framebuffer));

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.imageCount; framebufferIndex++) {
        debug("Framebuffer %d:", framebufferIndex);
        createFramebuffer(&framebufferSet.framebuffers[framebufferIndex], framebufferIndex);
        debug("\tSuccessfully created");
    }
}

void waitFramebufferDraw(Framebuffer *framebuffer) {
    vkWaitForFences(device, 1, &framebuffer->drawFence, VK_TRUE, UINT64_MAX);
    vkResetFences(  device, 1, &framebuffer->drawFence);
}

void waitFramebufferBlit(Framebuffer *framebuffer) {
    vkWaitForFences(device, 1, &framebuffer->blitFence, VK_TRUE, UINT64_MAX);
    vkResetFences(  device, 1, &framebuffer->blitFence);
}

void beginFramebuffer(Framebuffer *framebuffer) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };

    VkRenderingAttachmentInfo depthStencilAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = framebufferSet.depthStencil->view,
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
        .imageView = framebufferSet.color->view,
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
            .extent = surfaceExtent
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

void bindFramebuffer(Framebuffer *framebuffer) {
    VkViewport viewport = {
        .x = 0.0f,
        .y = (float) surfaceExtent.height,
        .width = (float) surfaceExtent.width,
        .height = - (float) surfaceExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {
            .x = 0,
            .y = 0
        },
        .extent = surfaceExtent
    };

    vkCmdSetViewportWithCount(framebuffer->renderCommandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount( framebuffer->renderCommandBuffer, 1, &scissor);
}

void endFramebuffer(Framebuffer *framebuffer) {
    vkCmdEndRendering( framebuffer->renderCommandBuffer);
    vkEndCommandBuffer(framebuffer->renderCommandBuffer);
}

void destroyFramebuffer(Framebuffer *framebuffer) {
    vkDestroyFence(device, framebuffer->blitFence, nullptr);
    vkDestroyFence(device, framebuffer->drawFence, nullptr);

    vkDestroySemaphore(device, framebuffer->blitSemaphore, nullptr);
    vkDestroySemaphore(device, framebuffer->drawSemaphore, nullptr);

    destroyImageView(framebuffer->resolve);
    destroyImage(    framebuffer->resolve);
}

void destroyFramebufferSet() {
    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.imageCount; framebufferIndex++) {
        destroyFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
        debug("Framebuffer %d destroyed", framebufferIndex);
    }

    destroyImageView(framebufferSet.color);
    destroyImageView(framebufferSet.depthStencil);

    destroyImage(framebufferSet.color);
    destroyImage(framebufferSet.depthStencil);

    resetDescriptorPool(&primitiveDescriptorPool);
    resetDescriptorPool(&sceneDescriptorPool);

    free(framebufferSet.framebuffers);

    frameMemory.offset = 0;
}
