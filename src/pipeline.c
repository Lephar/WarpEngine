#include "pipeline.h"

#include "physicalDevice.h"
#include "device.h"
#include "content.h"
#include "light.h"
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
    descriptorSetLayoutCount = 5;
    descriptorSetLayouts = malloc(descriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));

    descriptorSetLayouts[0] = lightingDescriptorPool.layout;
    descriptorSetLayouts[1] = cameraDescriptorPool.layout;
    descriptorSetLayouts[2] = primitiveDescriptorPool.layout;
    descriptorSetLayouts[3] = materialDescriptorPool.layout;
    descriptorSetLayouts[4] = samplerDescriptorPool.layout;

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

    lightingUniformAlignment  = sizeof(LightingUniform);

    cameraUniformAlignment    = align(sizeof(CameraUniform),    minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), minUniformBufferOffsetAlignment);
    materialUniformAlignment  = align(sizeof(MaterialUniform),  minUniformBufferOffsetAlignment);

    debug("Uniform Alignments:");
    debug("\tLighting:  %u", lightingUniformAlignment);
    debug("\tCamera:    %u", cameraUniformAlignment);
    debug("\tPrimitive: %u", primitiveUniformAlignment);
    debug("\tMaterial:  %u", materialUniformAlignment);

    lightingUniformBufferRange = maxUniformBufferRange;

    cameraUniformBufferRange    = alignBack(maxUniformBufferRange, cameraUniformAlignment);
    primitiveUniformBufferRange = alignBack(maxUniformBufferRange, primitiveUniformAlignment);
    materialUniformBufferRange  = alignBack(maxUniformBufferRange, materialUniformAlignment);

    debug("Uniform Buffer Ranges:");
    debug("\tLighting:  %u", lightingUniformBufferRange);
    debug("\tCamera:    %u", cameraUniformBufferRange);
    debug("\tPrimitive: %u", primitiveUniformBufferRange);
    debug("\tMaterial:  %u", materialUniformBufferRange);

    pointLightCountLimit = (lightingUniformBufferRange - lightingUniformAlignment) / sizeof(PointLightUniform);

    cameraCountLimit    = cameraUniformBufferRange    / cameraUniformAlignment;
    primitiveCountLimit = primitiveUniformBufferRange / primitiveUniformAlignment;
    materialCountLimit  = materialUniformBufferRange  / materialUniformAlignment;

    nodeCountLimit = primitiveCountLimit;

    debug("Count Limits:");
    debug("\tPoint Lights: %u", pointLightCountLimit);
    debug("\tCameras:      %u", cameraCountLimit);
    debug("\tPrimitives:   %u", primitiveCountLimit);
    debug("\tMaterials:    %u", materialCountLimit);
    debug("\tNodes:        %u", nodeCountLimit);

    framebufferUniformBufferSize    = lightingUniformBufferRange + cameraUniformBufferRange + primitiveUniformBufferRange + materialUniformBufferRange;
    framebufferSetUniformBufferSize = framebufferSetFramebufferCountLimit * framebufferUniformBufferSize;
}

void createPipeline() {
    createSampler();

    createBufferDescriptorPool(&lightingDescriptorPool,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferSetCountLimit * framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    createBufferDescriptorPool(&cameraDescriptorPool,    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferSetCountLimit * framebufferSetFramebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
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
    destroyDescriptorPool(&materialDescriptorPool);
    destroyDescriptorPool(&primitiveDescriptorPool);
    destroyDescriptorPool(&cameraDescriptorPool);
    destroyDescriptorPool(&lightingDescriptorPool);
    debug("Descriptor pools destroyed");

    vkDestroySampler(device, sampler, nullptr);
    debug("Texture sampler destroyed");

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    debug("Pipeline layout destroyed");
}
