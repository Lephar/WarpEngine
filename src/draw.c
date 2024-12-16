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

uint32_t frameCount;
uint32_t framebufferIndex;

void initializeDraw() {
    frameCount = 0;
    framebufferIndex = 0;

    debug("Draw loop started");
}

void render() {
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
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
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

    VkShaderStageFlags stages[] = {
          vertexShader.stage,
        fragmentShader.stage
    };

    VkShaderEXT shaders[] = {
          vertexShader.module,
        fragmentShader.module
    };

    uint32_t stageCount = sizeof(stages) / sizeof(VkShaderStageFlags);

    VkBool32 colorBlend = VK_FALSE;

    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT |
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT ;

    VkSampleMask sampleMask = 0xFF;

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

    // TODO: Whole fence logic...
    vkWaitForFences(device, 1, &framebuffer->renderFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &framebuffer->renderFence);

    vkBeginCommandBuffer(framebuffer->renderCommandBuffer, &beginInfo);
    vkCmdBeginRendering(framebuffer->renderCommandBuffer, &renderingInfo);

    vkCmdBindIndexBuffer(framebuffer->renderCommandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(framebuffer->renderCommandBuffer, 0, 1, &deviceBuffer.buffer, &indexBufferSize);

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

    PFN_vkCmdBindShadersEXT cmdBindShaders = loadFunction("vkCmdBindShadersEXT");
    cmdBindShaders(framebuffer->renderCommandBuffer, stageCount, stages, shaders);


    //vkCmdSetCullMode(framebuffer->commandBuffer, VK_CULL_MODE_BACK_BIT);
    //vkCmdSetFrontFace(framebuffer->commandBuffer, VK_FRONT_FACE_CLOCKWISE);

    //PFN_vkCmdSetPolygonModeEXT cmdSetPolygonMode = loadFunction("vkCmdSetPolygonModeEXT");
    //cmdSetPolygonMode(framebuffer->commandBuffer, VK_POLYGON_MODE_FILL);
    vkCmdSetPrimitiveTopology(framebuffer->renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    vkCmdSetPrimitiveRestartEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    //vkCmdSetDepthTestEnable(framebuffer->commandBuffer, VK_TRUE);
    //vkCmdSetDepthWriteEnable(framebuffer->commandBuffer, VK_TRUE);
    //vkCmdSetDepthBiasEnable(framebuffer->commandBuffer, VK_FALSE);
    //vkCmdSetDepthCompareOp(framebuffer->commandBuffer, VK_COMPARE_OP_GREATER);

    vkCmdSetStencilTestEnable(framebuffer->renderCommandBuffer, VK_FALSE);

    vkCmdSetRasterizerDiscardEnable(framebuffer->renderCommandBuffer, VK_TRUE);

    //PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadFunction("vkCmdSetRasterizationSamplesEXT");
    //cmdSetRasterizationSamples(framebuffer->commandBuffer, framebufferSet.sampleCount);
    //PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadFunction("vkCmdSetSampleMaskEXT");
    //cmdSetSampleMask(framebuffer->commandBuffer, framebufferSet.sampleCount, &sampleMask);

    //PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadFunction("vkCmdSetAlphaToOneEnableEXT");
    //cmdSetAlphaToOneEnable(framebuffer->commandBuffer, VK_FALSE);
    //PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadFunction("vkCmdSetAlphaToCoverageEnableEXT");
    //cmdSetAlphaToCoverageEnable(framebuffer->commandBuffer, VK_FALSE);
    //PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadFunction("vkCmdSetColorBlendEnableEXT");
    //cmdSetColorBlendEnable(framebuffer->commandBuffer, 0, 1, &colorBlend);

    // TODO: Why doesn't this work?
    //vkCmdSetViewport(framebuffer->commandBuffer, 0, 1, &viewport);
    //vkCmdSetScissor(framebuffer->commandBuffer, 0, 1, &scissor);

    vkCmdSetViewportWithCount(framebuffer->renderCommandBuffer, 1, &viewport);
    vkCmdSetScissorWithCount(framebuffer->renderCommandBuffer, 1, &scissor);

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(framebuffer->renderCommandBuffer, 1, &vertexBinding, 1, &vertexAttribute);


    vkCmdDrawIndexed(framebuffer->renderCommandBuffer, indexCount, 1, 0, 0, 0);

    vkCmdEndRendering(framebuffer->renderCommandBuffer);
    vkEndCommandBuffer(framebuffer->renderCommandBuffer);


    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = 0,
        .commandBufferCount = 1,
        .pCommandBuffers = &framebuffer->renderCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->drawSemaphore
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);

}

void present() { // TODO: WIP
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    uint32_t swapchainImageIndex = UINT32_MAX;

    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, framebuffer->acquireSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);

    VkImage *swapchainImage = &swapchain.images[swapchainImageIndex];

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

    vkBeginCommandBuffer(framebuffer->presentCommandBuffer, &beginInfo);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, &framebuffer->resolve.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    vkCmdBlitImage(framebuffer->presentCommandBuffer, framebuffer->resolve.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, *swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_NEAREST);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, &framebuffer->resolve.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    recordTransitionImageLayout(&framebuffer->presentCommandBuffer, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    vkEndCommandBuffer(framebuffer->presentCommandBuffer);

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore waitSemaphores[] = {
        framebuffer->acquireSemaphore,
        framebuffer->drawSemaphore
    };

    uint32_t semaphoreCount = sizeof(waitSemaphores) / sizeof(VkSemaphore);

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = semaphoreCount,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &framebuffer->presentCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->blitSemaphore
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, framebuffer->renderFence);

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &framebuffer->blitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.swapchain,
        .pImageIndices = &swapchainImageIndex
    };

    vkQueuePresentKHR(graphicsQueue.queue, &presentInfo);
}

void draw() {
    debug("%d", frameCount);

    framebufferIndex = frameCount % framebufferSet.framebufferImageCount;

    render();
    present();

    frameCount++;
}

void finalizeDraw() {
    debug("Draw loop ended");

    vkDeviceWaitIdle(device);
}
