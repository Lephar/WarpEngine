#include "draw.h"

#include "window.h"
#include "device.h"
#include "queue.h"
#include "image.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "content.h"
#include "shader.h"
#include "material.h"
#include "scene.h"
#include "primitive.h"

#include "logger.h"

void initializeDraw() {
    vkDeviceWaitIdle(device);

    updateProjection();

    debug("Draw loop started");
}

void render() {
    uint32_t framebufferIndex = frameIndex % framebufferSet.imageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    waitFramebufferDraw(framebuffer);

    updateUniforms(framebufferIndex);

    beginFramebuffer(&framebufferSet, framebuffer);
    bindFramebuffer(framebuffer);

    bindContentBuffers(framebuffer->renderCommandBuffer);
    bindPipeline(framebuffer->renderCommandBuffer);
    bindShaders(framebuffer->renderCommandBuffer, vertexShaderModule, fragmentShaderModule);
    bindScene(framebuffer->renderCommandBuffer, framebuffer->sceneDescriptorSet);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        Material *material = &materials[materialIndex];

        if(!material->isTransparent) {
            bindMaterial(framebuffer->renderCommandBuffer, material);

            for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
                Primitive *primitive = &primitives[primitiveIndex];

                if(primitive->material == material) {
                    drawPrimitive(framebuffer->renderCommandBuffer, framebuffer->primitiveDescriptorSet, primitive);
                }
            }
        }
    }

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        Material *material = &materials[materialIndex];

        if(material->isTransparent) {
            bindMaterial(framebuffer->renderCommandBuffer, material);

            for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
                Primitive *primitive = &primitives[primitiveIndex];

                if(primitive->material == material) {
                    drawPrimitive(framebuffer->renderCommandBuffer, framebuffer->primitiveDescriptorSet, primitive);
                }
            }
        }
    }

    endFramebuffer(framebuffer);

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &framebuffer->blitSemaphore,
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

    waitFramebufferBlit(framebuffer);

    uint32_t swapchainImageIndex = UINT32_MAX;
    VkResult acquisitionResult = vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, framebuffer->acquireSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);
    assert(acquisitionResult != VK_TIMEOUT && acquisitionResult != VK_NOT_READY);

    if(acquisitionResult == VK_ERROR_OUT_OF_HOST_MEMORY    ||
       acquisitionResult == VK_ERROR_OUT_OF_DEVICE_MEMORY  ||
       acquisitionResult == VK_ERROR_DEVICE_LOST           ||
       acquisitionResult == VK_ERROR_UNKNOWN               ||
       acquisitionResult == VK_ERROR_OUT_OF_DATE_KHR       ||
       acquisitionResult == VK_ERROR_SURFACE_LOST_KHR      ||
       acquisitionResult == VK_ERROR_VALIDATION_FAILED_EXT ||
       acquisitionResult == VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT) {
        debug("Hard error on swapchain image acquisition, skipping");
        // TODO: Signal necessary semaphores and fences here
        return;
    } else if(acquisitionResult == VK_SUBOPTIMAL_KHR) {
        debug("Suboptimal swapchain image, blitting anyway");
    }

    assert(swapchainImageIndex < swapchain.imageCount);

    Image *swapchainImage = &swapchain.images[swapchainImageIndex];
    VkSemaphore *presentSemaphore = &swapchain.presentSemaphores[swapchainImageIndex];

    vkBeginCommandBuffer(framebuffer->presentCommandBuffer, &beginInfo);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, framebuffer->resolve, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkCmdBlitImage(framebuffer->presentCommandBuffer, framebuffer->resolve->image, framebuffer->resolve->layout, swapchainImage->image, swapchainImage->layout, 1, &region, VK_FILTER_NEAREST);
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
        framebuffer->blitSemaphore,
        *presentSemaphore
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
        .pWaitSemaphores = presentSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &swapchainImageIndex
    };

    VkResult presentationResult = vkQueuePresentKHR(graphicsQueue.queue, &presentInfo);

    if(presentationResult == VK_ERROR_DEVICE_LOST           ||
       presentationResult == VK_ERROR_OUT_OF_DEVICE_MEMORY  ||
       presentationResult == VK_ERROR_OUT_OF_HOST_MEMORY    ||
       presentationResult == VK_ERROR_UNKNOWN               ||
       presentationResult == VK_ERROR_OUT_OF_DATE_KHR       ||
       presentationResult == VK_ERROR_SURFACE_LOST_KHR      ||
       presentationResult == VK_ERROR_VALIDATION_FAILED_EXT ||
       presentationResult == VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT) {
        debug("Hard error on swapchain image presentation, skipping");
        // TODO: Signal necessary semaphores and fences here
        return;
    } else if(acquisitionResult == VK_SUBOPTIMAL_KHR) {
        debug("Suboptimal swapchain image, presenting anyway");
    }
}

// TODO: Fix synchronization errors and simplify logic
void draw() {
    render();
    present();
}

void finalizeDraw() {
    debug("Draw loop ended");

    vkDeviceWaitIdle(device);
}
