#include "draw.h"

#include "window.h"
#include "device.h"
#include "queue.h"
#include "buffer.h"
#include "image.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "content.h"
#include "shader.h"
#include "material.h"
#include "meta.h"
#include "primitive.h"

#include "logger.h"

void initializeDraw() {
    vkDeviceWaitIdle(device);

    debug("Draw loop started");
}

void render() {
    uint32_t framebufferIndex = frameIndex % framebufferSet.imageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    waitFramebuffer(framebuffer);

    beginFramebuffer(framebuffer);
    bindFramebuffer(framebuffer);

    bindContentBuffers(framebuffer->renderCommandBuffer);
    bindPipeline(framebuffer->renderCommandBuffer);

    bindShaders(framebuffer->renderCommandBuffer, &skyboxShaderModule, &fragmentShaderModule);

    bindScene(framebuffer->renderCommandBuffer, framebuffer->sceneDescriptorSet);

    bindMaterial(framebuffer->renderCommandBuffer, skybox->material);
    drawPrimitive(framebuffer->renderCommandBuffer, framebuffer->primitiveDescriptorSet, &skybox);

    bindShaders(framebuffer->renderCommandBuffer, &vertexShaderModule, &fragmentShaderModule);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        Material *material = &materials[materialIndex];

        bindMaterial(framebuffer->renderCommandBuffer, material);

        for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
            Primitive *primitive = &primitives[primitiveIndex];

            if(primitive->material == material) {
                drawPrimitive(framebuffer->renderCommandBuffer, framebuffer->primitiveDescriptorSet, primitive);
            }
        }
    }

    endFramebuffer(framebuffer);

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &framebuffer->blitSemaphoreRender,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &framebuffer->renderCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->drawSemaphore
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, framebuffer->drawFence);
}

void present() {
    uint32_t framebufferIndex = frameIndex % framebufferSet.imageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    VkImageBlit region = {
        .srcSubresource = {
            .aspectMask = framebuffer->resolve->aspect,
            .mipLevel = 0,
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
                .x = framebuffer->resolve->extent.width,
                .y = framebuffer->resolve->extent.height,
                .z = 1
            }
        },
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
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
                .x = extent.width,
                .y = extent.height,
                .z = 1
            }
        },
    };

    vkWaitForFences(device, 1, &framebuffer->blitFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &framebuffer->blitFence);

    uint32_t swapchainImageIndex = UINT32_MAX;
    VkResult swapchainStatus = vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, framebuffer->acquireSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);

    if( swapchainStatus == VK_ERROR_OUT_OF_HOST_MEMORY   ||
        swapchainStatus == VK_ERROR_OUT_OF_DEVICE_MEMORY ||
        swapchainStatus == VK_ERROR_DEVICE_LOST          ||
        swapchainStatus == VK_ERROR_OUT_OF_DATE_KHR      ||
        swapchainStatus == VK_ERROR_SURFACE_LOST_KHR     ||
        swapchainStatus == VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT) {
        debug("Hard error on swapchain image acquisition, skipping");
        return;
    } else if(  swapchainStatus == VK_TIMEOUT ||
                swapchainStatus == VK_NOT_READY) {
        debug("No swapchain image ready for the acquisition, skipping");
        return;
    } else if(swapchainStatus == VK_SUBOPTIMAL_KHR) {
        debug("Suboptimal swapchain image, drawing anyway");
    }

    // TODO: Can this happen when no error code returned?
    if(swapchainImageIndex >= swapchain.imageCount) {
        debug("Faulty swapchain image index returned, this shouldn't happen");
        return;
    }

    Image *swapchainImage = &swapchain.images[swapchainImageIndex];

    vkBeginCommandBuffer(framebuffer->presentCommandBuffer, &beginInfo);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, framebuffer->resolve, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkCmdBlitImage(framebuffer->presentCommandBuffer, framebuffer->resolve->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainImage->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_NEAREST);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, framebuffer->resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    vkEndCommandBuffer(framebuffer->presentCommandBuffer);

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT
    };

    VkSemaphore waitSemaphores[] = {
        framebuffer->acquireSemaphore,
        framebuffer->drawSemaphore
    };

    uint32_t waitSemaphoreCount = sizeof(waitSemaphores) / sizeof(VkSemaphore);

    VkSemaphore signalSemaphores[] = {
        framebuffer->blitSemaphoreRender,
        framebuffer->blitSemaphorePresent
    };

    uint32_t signalSemaphoreCount = sizeof(signalSemaphores) / sizeof(VkSemaphore);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = waitSemaphoreCount,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &framebuffer->presentCommandBuffer,
        .signalSemaphoreCount = signalSemaphoreCount,
        .pSignalSemaphores = signalSemaphores
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, framebuffer->blitFence);

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &framebuffer->blitSemaphorePresent,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &swapchainImageIndex
    };

    vkQueuePresentKHR(graphicsQueue.queue, &presentInfo);
}

void draw() {
    render();
    present();
}

void finalizeDraw() {
    debug("Draw loop ended");

    vkDeviceWaitIdle(device);
}
