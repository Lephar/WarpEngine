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
#include "primitive.h"

#include "logger.h"

void initializeDraw() {
    vkDeviceWaitIdle(device);

    debug("Draw loop started");
}

void render() {
    uint32_t framebufferIndex = frameIndex % framebufferSet.imageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL
    };

    VkRenderingAttachmentInfo depthStencilAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = NULL,
        .imageView = framebuffer->depthStencil->view,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
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
        .pNext = NULL,
        .imageView = framebuffer->color->view,
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
        .pNext = NULL,
        .flags = 0,
        .renderArea = {
            .offset = {
                .x = 0,
                .y = 0,
            },
            .extent = extent
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentInfo,
        .pDepthAttachment = &depthStencilAttachmentInfo,
        .pStencilAttachment = &depthStencilAttachmentInfo
    };

    VkSampleMask sampleMask = 0xFF;

    VkBool32 colorBlend = VK_FALSE;

    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT |
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT ;

    VkViewport viewport = {
        .x = 0.0f,
        .y = (float) extent.height,
        .width = (float) extent.width,
        .height = - (float) extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {
            .x = 0,
            .y = 0
        },
        .extent = extent
    };

    VkVertexInputBindingDescription2EXT vertexBinding = {
        .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .pNext = NULL,
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor = 1
    };

    VkVertexInputAttributeDescription2EXT vertexAttributes[] = {
        {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = 0
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = sizeof(vec3)
        }
    };

    uint32_t vertexAttributeCount = sizeof(vertexAttributes) / sizeof(VkVertexInputAttributeDescription2EXT);

    VkShaderStageFlags stages[] = {
          vertexShaderModule.stage,
        fragmentShaderModule.stage
    };

    VkShaderEXT skyboxShaders[] = {
          skyboxShaderModule.module,
        fragmentShaderModule.module
    };

    VkShaderEXT pipelineShaders[] = {
          vertexShaderModule.module,
        fragmentShaderModule.module
    };

    uint32_t stageCount = sizeof(stages) / sizeof(VkShaderStageFlags);

    vkWaitForFences(device, 1, &framebuffer->drawFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &framebuffer->drawFence);

    vkBeginCommandBuffer(framebuffer->renderCommandBuffer, &beginInfo);
    vkCmdBeginRendering(framebuffer->renderCommandBuffer, &renderingInfo);

    vkCmdBindIndexBuffer(framebuffer->renderCommandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(framebuffer->renderCommandBuffer, 0, 1, &deviceBuffer.buffer, &indexBufferSize);

    vkCmdSetRasterizerDiscardEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    vkCmdSetCullMode(framebuffer->renderCommandBuffer, VK_CULL_MODE_BACK_BIT);
    vkCmdSetFrontFace(framebuffer->renderCommandBuffer, VK_FRONT_FACE_COUNTER_CLOCKWISE);

    vkCmdSetDepthTestEnable(framebuffer->renderCommandBuffer, VK_TRUE);
    vkCmdSetDepthWriteEnable(framebuffer->renderCommandBuffer, VK_TRUE);
    vkCmdSetDepthBiasEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    vkCmdSetDepthCompareOp(framebuffer->renderCommandBuffer, VK_COMPARE_OP_LESS);

    vkCmdSetStencilTestEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    vkCmdSetPrimitiveRestartEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    vkCmdSetPrimitiveTopology(framebuffer->renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    PFN_vkCmdSetPolygonModeEXT cmdSetPolygonMode = loadDeviceFunction("vkCmdSetPolygonModeEXT");
    cmdSetPolygonMode(framebuffer->renderCommandBuffer, VK_POLYGON_MODE_FILL);
    //cmdSetPolygonMode(framebuffer->renderCommandBuffer, VK_POLYGON_MODE_LINE);
    //vkCmdSetLineWidth(framebuffer->renderCommandBuffer, 1.0f);

    PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadDeviceFunction("vkCmdSetRasterizationSamplesEXT");
    cmdSetRasterizationSamples(framebuffer->renderCommandBuffer, framebufferSet.sampleCount);
    PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadDeviceFunction("vkCmdSetSampleMaskEXT");
    cmdSetSampleMask(framebuffer->renderCommandBuffer, framebufferSet.sampleCount, &sampleMask);

    PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadDeviceFunction("vkCmdSetAlphaToOneEnableEXT");
    cmdSetAlphaToOneEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadDeviceFunction("vkCmdSetAlphaToCoverageEnableEXT");
    cmdSetAlphaToCoverageEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadDeviceFunction("vkCmdSetColorBlendEnableEXT");
    cmdSetColorBlendEnable(framebuffer->renderCommandBuffer, 0, 1, &colorBlend);
    PFN_vkCmdSetColorWriteMaskEXT cmdSetColorWriteMask = loadDeviceFunction("vkCmdSetColorWriteMaskEXT");
    cmdSetColorWriteMask(framebuffer->renderCommandBuffer, 0, 1, &colorWriteMask);

    vkCmdSetViewportWithCount(framebuffer->renderCommandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount(framebuffer->renderCommandBuffer, 1, &scissor);

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadDeviceFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(framebuffer->renderCommandBuffer, 1, &vertexBinding, vertexAttributeCount, vertexAttributes);

    PFN_vkCmdBindShadersEXT cmdBindShaders = loadDeviceFunction("vkCmdBindShadersEXT");

    cmdBindShaders(framebuffer->renderCommandBuffer, stageCount, stages, skyboxShaders);

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &primitives[0].material->samplerDescriptor, 0, NULL);
    vkCmdDrawIndexed(framebuffer->renderCommandBuffer, primitives[0].indexCount, 1, primitives[0].indexBegin, primitives[0].vertexOffset, 0);

    cmdBindShaders(framebuffer->renderCommandBuffer, stageCount, stages, pipelineShaders);

    for(uint32_t primitiveIndex = 1; primitiveIndex < primitiveCount; primitiveIndex++) {
        vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &primitives[primitiveIndex].material->samplerDescriptor, 0, NULL);
        vkCmdDrawIndexed(framebuffer->renderCommandBuffer, primitives[primitiveIndex].indexCount, 1, primitives[primitiveIndex].indexBegin, primitives[primitiveIndex].vertexOffset, 0);
    }

    vkCmdEndRendering(framebuffer->renderCommandBuffer);
    vkEndCommandBuffer(framebuffer->renderCommandBuffer);

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
