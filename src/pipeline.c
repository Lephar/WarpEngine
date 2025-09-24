#include "pipeline.h"

#include "physicalDevice.h"
#include "device.h"
#include "content.h"
#include "camera.h"
#include "asset.h"
#include "primitive.h"
#include "material.h"
#include "descriptor.h"
#include "framebuffer.h"

#include "logger.h"
#include "numerics.h"

uint32_t descriptorSetLayoutCount;
VkDescriptorSetLayout *descriptorSetLayouts;
VkPipelineLayout pipelineLayout;

void createPipelineLayout() {
    descriptorSetLayoutCount = 4;
    descriptorSetLayouts = malloc(descriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));

    descriptorSetLayouts[0] = cameraDescriptorPool.layout;
    descriptorSetLayouts[1] = primitiveDescriptorPool.layout;
    descriptorSetLayouts[2] = materialDescriptorPool.layout;
    descriptorSetLayouts[3] = samplerDescriptorPool.layout;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = descriptorSetLayoutCount,
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    debug("Pipeline layout created");
}

void setPipelineDetails() {
    const uint32_t minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    const uint32_t maxUniformBufferRange = umin(physicalDeviceProperties.limits.maxUniformBufferRange, USHRT_MAX + 1);

    cameraUniformAlignment    = align(sizeof(Camera),           minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), minUniformBufferOffsetAlignment);
    materialUniformAlignment  = align(sizeof(MaterialUniform),  minUniformBufferOffsetAlignment);

    cameraUniformBufferRange    = cameraUniformAlignment;
    primitiveUniformBufferRange = alignBack(maxUniformBufferRange, primitiveUniformAlignment);
    materialUniformBufferRange  = alignBack(maxUniformBufferRange, materialUniformAlignment);

    cameraCountLimit = framebufferSetCountLimit;

    primitiveCountLimit = primitiveUniformBufferRange / primitiveUniformAlignment;
    materialCountLimit  = materialUniformBufferRange  / materialUniformAlignment;

    nodeCountLimit = umax(primitiveCountLimit, materialCountLimit);

    framebufferUniformBufferSize    = cameraUniformBufferRange + primitiveUniformBufferRange + materialUniformBufferRange;
    framebufferSetUniformBufferSize = framebufferSetFramebufferCountLimit * framebufferUniformBufferSize;
}

void createPipeline() {
    createSampler();

    createBufferDescriptorPool(&cameraDescriptorPool,    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferSetCountLimit * framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    createBufferDescriptorPool(&primitiveDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferSetCountLimit * framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT);
    createBufferDescriptorPool(&materialDescriptorPool,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferSetCountLimit * framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_FRAGMENT_BIT);

    createSamplerDescriptorPool(&samplerDescriptorPool,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCountLimit, VK_SHADER_STAGE_FRAGMENT_BIT);

    createPipelineLayout();
}

void bindPipeline(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT |
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT ;

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
    //cmdSetPolygonMode(framebuffer->renderCommandBuffer, VK_POLYGON_MODE_POINT);

    PFN_vkCmdSetColorWriteMaskEXT cmdSetColorWriteMask = loadDeviceFunction("vkCmdSetColorWriteMaskEXT");
    cmdSetColorWriteMask(framebuffer->renderCommandBuffer, 0, 1, &colorWriteMask);

    PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadDeviceFunction("vkCmdSetAlphaToOneEnableEXT");
    cmdSetAlphaToOneEnable(framebuffer->renderCommandBuffer, VK_FALSE);
    PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadDeviceFunction("vkCmdSetAlphaToCoverageEnableEXT");
    cmdSetAlphaToCoverageEnable(framebuffer->renderCommandBuffer, VK_FALSE);
}

void destroyPipeline() {
    destroyDescriptorPool(&samplerDescriptorPool);
    destroyDescriptorPool(&cameraDescriptorPool);
    destroyDescriptorPool(&primitiveDescriptorPool);
    destroyDescriptorPool(&materialDescriptorPool);
    debug("Descriptor pools destroyed");

    vkDestroySampler(device, sampler, nullptr);
    debug("Texture sampler destroyed");

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    debug("Pipeline layout destroyed");
}
