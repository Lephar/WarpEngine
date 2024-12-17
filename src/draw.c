#include "draw.h"

#include "helper.h"

#include "queue.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "buffer.h"
#include "element.h"
#include "shader.h"

extern VkDevice device;

extern VkExtent2D extent;

extern Queue graphicsQueue;

extern Swapchain swapchain;
extern FramebufferSet framebufferSet;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedBuffer;

extern uint32_t  indexCount;
extern uint32_t vertexCount;

extern VkDeviceSize   indexBufferSize;
extern VkDeviceSize  vertexBufferSize;
extern VkDeviceSize uniformBufferSize;

extern Index    *  indexBuffer;
extern Vertex   * vertexBuffer;
extern Uniform  *uniformBuffer;

extern VkDescriptorSet descriptorSet;
extern VkPipelineLayout pipelineLayout;

extern Shader   vertexShader;
extern Shader fragmentShader;

extern uint32_t frameIndex;

void initializeDraw() {
    debug("Draw loop started");
}

void render() {
    uint32_t framebufferIndex = frameIndex % framebufferSet.framebufferImageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    VkRenderingAttachmentInfo depthStencilAttachmentInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = NULL,
        .imageView = framebuffer->depthStencil.view,
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
        .imageView = framebuffer->color.view,
        .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
        .resolveImageView = framebuffer->resolve.view,
        .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {
            .color = {
                .float32 = {
                    1.0f,
                    0.0f,
                    1.0f,
                    1.0f
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
        .x = 0,
        .y = 0,
        .width = extent.width,
        .height = extent.height,
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

    VkVertexInputAttributeDescription2EXT vertexAttribute = {
        .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
        .pNext = NULL,
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0
    };

    VkShaderStageFlags stages[] = {
          vertexShader.stage,
        fragmentShader.stage
    };

    VkShaderEXT shaders[] = {
          vertexShader.module,
        fragmentShader.module
    };

    uint32_t stageCount = sizeof(stages) / sizeof(VkShaderStageFlags);

    vkWaitForFences(device, 1, &framebuffer->drawFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &framebuffer->drawFence);

    vkBeginCommandBuffer(framebuffer->renderCommandBuffer, &beginInfo);
    vkCmdBeginRendering(framebuffer->renderCommandBuffer, &renderingInfo);

    vkCmdBindIndexBuffer(framebuffer->renderCommandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(framebuffer->renderCommandBuffer, 0, 1, &deviceBuffer.buffer, &indexBufferSize);

    vkCmdSetRasterizerDiscardEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    vkCmdSetCullMode(framebuffer->renderCommandBuffer, VK_CULL_MODE_NONE);
    vkCmdSetFrontFace(framebuffer->renderCommandBuffer, VK_FRONT_FACE_COUNTER_CLOCKWISE);

    vkCmdSetDepthTestEnable(framebuffer->renderCommandBuffer, VK_TRUE);
    vkCmdSetDepthWriteEnable(framebuffer->renderCommandBuffer, VK_TRUE);
    vkCmdSetDepthBiasEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    vkCmdSetDepthCompareOp(framebuffer->renderCommandBuffer, VK_COMPARE_OP_ALWAYS);

    vkCmdSetStencilTestEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    vkCmdSetPrimitiveRestartEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    vkCmdSetPrimitiveTopology(framebuffer->renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    PFN_vkCmdSetPolygonModeEXT cmdSetPolygonMode = loadFunction("vkCmdSetPolygonModeEXT");
    cmdSetPolygonMode(framebuffer->renderCommandBuffer, VK_POLYGON_MODE_FILL);

    PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadFunction("vkCmdSetRasterizationSamplesEXT");
    cmdSetRasterizationSamples(framebuffer->renderCommandBuffer, framebufferSet.sampleCount);
    PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadFunction("vkCmdSetSampleMaskEXT");
    cmdSetSampleMask(framebuffer->renderCommandBuffer, framebufferSet.sampleCount, &sampleMask);

    PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadFunction("vkCmdSetAlphaToOneEnableEXT");
    cmdSetAlphaToOneEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadFunction("vkCmdSetAlphaToCoverageEnableEXT");
    cmdSetAlphaToCoverageEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadFunction("vkCmdSetColorBlendEnableEXT");
    cmdSetColorBlendEnable(framebuffer->renderCommandBuffer, 0, 1, &colorBlend);
    PFN_vkCmdSetColorWriteMaskEXT cmdSetColorWriteMask = loadFunction("vkCmdSetColorWriteMaskEXT");
    cmdSetColorWriteMask(framebuffer->renderCommandBuffer, 0, 1, &colorWriteMask);

    // TODO: Why doesn't this work?
    //vkCmdSetViewport(framebuffer->renderCommandBuffer, 0, 1, &viewport);
    //vkCmdSetScissor(framebuffer->renderCommandBuffer, 0, 1, &scissor);

    vkCmdSetViewportWithCount(framebuffer->renderCommandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount(framebuffer->renderCommandBuffer, 1, &scissor);

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(framebuffer->renderCommandBuffer, 1, &vertexBinding, 1, &vertexAttribute);

    PFN_vkCmdBindShadersEXT cmdBindShaders = loadFunction("vkCmdBindShadersEXT");
    cmdBindShaders(framebuffer->renderCommandBuffer, stageCount, stages, shaders);

    vkCmdDrawIndexed(framebuffer->renderCommandBuffer, indexCount, 1, 0, 0, 0);

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
    uint32_t framebufferIndex = frameIndex % framebufferSet.framebufferImageCount;
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL
    };

    VkImageBlit region = {
        .srcSubresource = {
            .aspectMask = framebuffer->resolve.aspects,
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
                .x = framebuffer->resolve.extent.width,
                .y = framebuffer->resolve.extent.height,
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

    // TODO: Investigate multiple swapchain image acquisition at startup
    uint32_t swapchainImageIndex = UINT32_MAX;
    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, framebuffer->acquireSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);
    Image *swapchainImage = &swapchain.images[swapchainImageIndex];

    vkBeginCommandBuffer(framebuffer->presentCommandBuffer, &beginInfo);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, &framebuffer->resolve, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkCmdBlitImage(framebuffer->presentCommandBuffer, framebuffer->resolve.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchainImage->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_NEAREST);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, &framebuffer->resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
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
