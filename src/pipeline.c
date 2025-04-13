#include "pipeline.h"

#include "physicalDevice.h"
#include "device.h"
#include "content.h"
#include "descriptor.h"
#include "framebuffer.h"

#include "logger.h"
#include "numerics.h"

VkPipelineLayout pipelineLayout;

void createPipelineLayout() {
    VkDescriptorSetLayout descriptorSetLayouts[] = {
        sceneDescriptorPool.layout,
        primitiveDescriptorPool.layout,
        materialDescriptorPool.layout
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = sizeof(descriptorSetLayouts) / sizeof(VkDescriptorSetLayout),
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout);
    debug("Pipeline layout created");
}

void createPipeline() {
    sceneUniformAlignment     = align(sizeof(SceneUniform),     physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    dynamicUniformBufferRange = alignBack(umin(physicalDeviceProperties.limits.maxUniformBufferRange, USHRT_MAX + 1), primitiveUniformAlignment);
    framebufferUniformStride  = sceneUniformAlignment + dynamicUniformBufferRange;
    primitiveCountLimit       = dynamicUniformBufferRange / primitiveUniformAlignment;

    createSampler();

    sceneDescriptorPool     = createDescriptorPool(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT);
    primitiveDescriptorPool = createDescriptorPool(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT);
    materialDescriptorPool  = createDescriptorPool(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, primitiveCountLimit,   VK_SHADER_STAGE_FRAGMENT_BIT);

    createPipelineLayout();
}

void bindPipeline(VkCommandBuffer commandBuffer) {
    VkSampleMask sampleMask = 0xFF;

    VkBool32 colorBlend = VK_FALSE;

    VkColorComponentFlags colorWriteMask =
        VK_COLOR_COMPONENT_A_BIT |
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT ;

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

    PFN_vkCmdSetRasterizationSamplesEXT cmdSetRasterizationSamples = loadDeviceFunction("vkCmdSetRasterizationSamplesEXT");
    cmdSetRasterizationSamples(commandBuffer, framebufferSet.sampleCount);
    PFN_vkCmdSetSampleMaskEXT cmdSetSampleMask = loadDeviceFunction("vkCmdSetSampleMaskEXT");
    cmdSetSampleMask(commandBuffer, framebufferSet.sampleCount, &sampleMask);

    PFN_vkCmdSetAlphaToOneEnableEXT cmdSetAlphaToOneEnable = loadDeviceFunction("vkCmdSetAlphaToOneEnableEXT");
    cmdSetAlphaToOneEnable(commandBuffer, VK_FALSE);
    PFN_vkCmdSetAlphaToCoverageEnableEXT cmdSetAlphaToCoverageEnable = loadDeviceFunction("vkCmdSetAlphaToCoverageEnableEXT");
    cmdSetAlphaToCoverageEnable(commandBuffer, VK_FALSE);

    PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadDeviceFunction("vkCmdSetColorBlendEnableEXT");
    cmdSetColorBlendEnable(commandBuffer, 0, 1, &colorBlend);
    PFN_vkCmdSetColorWriteMaskEXT cmdSetColorWriteMask = loadDeviceFunction("vkCmdSetColorWriteMaskEXT");
    cmdSetColorWriteMask(commandBuffer, 0, 1, &colorWriteMask);
}

void destroyPipeline() {
    vkDestroyDescriptorPool(device, materialDescriptorPool,  NULL);
    vkDestroyDescriptorPool(device, primitiveDescriptorPool, NULL);
    vkDestroyDescriptorPool(device, sceneDescriptorPool,     NULL);
    debug("Descriptor pools destroyed");

    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    debug("Pipeline layout destroyed");

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");

    vkDestroySampler(device, sampler, NULL);
    debug("Texture sampler destroyed");
}
