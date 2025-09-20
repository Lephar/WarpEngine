#include "pipeline.h"

#include "physicalDevice.h"
#include "device.h"
#include "content.h"
#include "scene.h"
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

    descriptorSetLayouts[0] = materialDescriptorPool.layout;
    descriptorSetLayouts[1] = factorDescriptorPool.layout;
    descriptorSetLayouts[2] = primitiveDescriptorPool.layout;
    descriptorSetLayouts[3] = sceneDescriptorPool.layout;

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
    uint32_t minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    uint32_t maxUniformBufferRange = umin(physicalDeviceProperties.limits.maxUniformBufferRange, USHRT_MAX + 1);

    sceneUniformAlignment     = align(sizeof(SceneUniform),     minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), minUniformBufferOffsetAlignment);
    materialUniformAlignment  = align(sizeof(MaterialUniform),  minUniformBufferOffsetAlignment);

    primitiveUniformBufferRange = alignBack(maxUniformBufferRange, primitiveUniformAlignment);
    materialUniformBufferRange  = alignBack(maxUniformBufferRange, materialUniformAlignment);

    primitiveCountLimit = primitiveUniformBufferRange / primitiveUniformAlignment;
    materialCountLimit  = materialUniformBufferRange  / materialUniformAlignment;

    framebufferSetUniformBufferOffset = materialUniformBufferRange;
    framebufferUniformBufferStride = primitiveUniformBufferRange + sceneUniformAlignment;
}

void createPipeline() {
    createSampler();

    createSamplerDescriptorPool(&materialDescriptorPool,  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCountLimit,                  VK_SHADER_STAGE_FRAGMENT_BIT);
    createBufferDescriptorPool( &factorDescriptorPool,    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1,                             VK_SHADER_STAGE_FRAGMENT_BIT);
    createBufferDescriptorPool( &primitiveDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT);
    createBufferDescriptorPool( &sceneDescriptorPool,     VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

    createPipelineLayout();
}

void bindPipeline(VkCommandBuffer commandBuffer) {
    VkSampleMask sampleMask = 0xFF;

    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT |
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT ;

    VkBool32 colorBlend = VK_TRUE;

    VkColorBlendEquationEXT colorBlendEquations = {
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    vkCmdSetRasterizerDiscardEnable(commandBuffer, VK_FALSE);

    vkCmdSetCullMode(commandBuffer, VK_CULL_MODE_BACK_BIT);
    vkCmdSetFrontFace(commandBuffer, VK_FRONT_FACE_COUNTER_CLOCKWISE);

    vkCmdSetDepthTestEnable(commandBuffer, VK_TRUE);
    vkCmdSetDepthWriteEnable(commandBuffer, VK_TRUE);
    vkCmdSetDepthBiasEnable(commandBuffer, VK_FALSE);
    vkCmdSetDepthCompareOp(commandBuffer, VK_COMPARE_OP_LESS);

    vkCmdSetStencilTestEnable(commandBuffer, VK_FALSE);

    vkCmdSetPrimitiveRestartEnable(commandBuffer, VK_FALSE);
    vkCmdSetPrimitiveTopology(commandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    PFN_vkCmdSetPolygonModeEXT cmdSetPolygonMode = loadDeviceFunction("vkCmdSetPolygonModeEXT");
    cmdSetPolygonMode(commandBuffer, VK_POLYGON_MODE_FILL);
    //cmdSetPolygonMode(commandBuffer, VK_POLYGON_MODE_LINE);
    //vkCmdSetLineWidth(commandBuffer, 1.0f);
    //cmdSetPolygonMode(commandBuffer, VK_POLYGON_MODE_POINT);

    PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadDeviceFunction("vkCmdSetRasterizationSamplesEXT");
    cmdSetRasterizationSamples(commandBuffer, framebufferSet.sampleCount);
    PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadDeviceFunction("vkCmdSetSampleMaskEXT");
    cmdSetSampleMask(commandBuffer, framebufferSet.sampleCount, &sampleMask);

    PFN_vkCmdSetColorWriteMaskEXT cmdSetColorWriteMask = loadDeviceFunction("vkCmdSetColorWriteMaskEXT");
    cmdSetColorWriteMask(commandBuffer, 0, 1, &colorWriteMask);

    PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadDeviceFunction("vkCmdSetColorBlendEnableEXT");
    cmdSetColorBlendEnable(commandBuffer, 0, 1, &colorBlend);
    PFN_vkCmdSetColorBlendEquationEXT cmdSetColorBlendEquation = loadDeviceFunction("vkCmdSetColorBlendEquationEXT");
    cmdSetColorBlendEquation(commandBuffer, 0, 1, &colorBlendEquations);

    PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadDeviceFunction("vkCmdSetAlphaToOneEnableEXT");
    cmdSetAlphaToOneEnable(commandBuffer, VK_FALSE);
    PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadDeviceFunction("vkCmdSetAlphaToCoverageEnableEXT");
    cmdSetAlphaToCoverageEnable(commandBuffer, VK_FALSE);
}

void destroyPipeline() {
    destroyDescriptorPool(&materialDescriptorPool);
    destroyDescriptorPool(&factorDescriptorPool);
    destroyDescriptorPool(&primitiveDescriptorPool);
    destroyDescriptorPool(&sceneDescriptorPool);
    debug("Descriptor pools destroyed");

    vkDestroySampler(device, sampler, nullptr);
    debug("Texture sampler destroyed");

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    debug("Pipeline layout destroyed");
}
