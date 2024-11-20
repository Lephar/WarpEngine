#include "render.h"

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

void initializeRender() {
    frameCount = 0;
    framebufferIndex = 0;
}

void record() {
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
                    0.0f,
                    0.0f,
                    0.0f,
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

    //VkBool32 colorBlend = VK_FALSE;

    //VkSampleMask sampleMask = 0;

    VkViewport viewport = {
        .x = 0,
        .y = 0,
        .width = extent.width,
        .height = extent.height
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

    vkWaitForFences(device, 1, &framebuffer->fence, VK_TRUE, UINT64_MAX);

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
}

void submit() {
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = 0,
        .commandBufferCount = 1,
        .pCommandBuffers = &framebuffer->renderCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &framebuffer->finishedSemaphore
    };

    vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);
}

void present() {
    Framebuffer *framebuffer = &framebufferSet.framebuffers[framebufferIndex];

    uint32_t swapchainImageIndex = UINT32_MAX;

    vkAcquireNextImageKHR(device, swapchain.swapchain, UINT64_MAX, framebuffer->acquireSemaphore, VK_NULL_HANDLE, &swapchainImageIndex);


}

void render() {
    framebufferIndex = frameCount % framebufferSet.framebufferImageCount;

    record();
    submit();
    present();
}
