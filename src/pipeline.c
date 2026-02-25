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

uint32_t pushConstantRangeCount;
VkPushConstantRange *pushConstantRanges;

VkPipelineLayout pipelineLayout;

void createPipelineLayout() {
    descriptorSetLayoutCount = 6;
    pushConstantRangeCount   = 1;

    descriptorSetLayouts = malloc(descriptorSetLayoutCount * sizeof(VkDescriptorSetLayout));
    pushConstantRanges   = malloc(pushConstantRangeCount   * sizeof(VkPushConstantRange));

    descriptorSetLayouts[0] = lightingDescriptorPool.layout;
    descriptorSetLayouts[1] = cameraDescriptorPool.layout;
    descriptorSetLayouts[2] = primitiveDescriptorPool.layout;
    descriptorSetLayouts[3] = materialDescriptorPool.layout;
    descriptorSetLayouts[4] = samplerDescriptorPool.layout;
    descriptorSetLayouts[5] = storageDescriptorPool.layout;

    pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRanges[0].offset = 0;
    pushConstantRanges[0].size   = sizeof(int32_t);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = descriptorSetLayoutCount,
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = pushConstantRangeCount,
        .pPushConstantRanges = pushConstantRanges,
    };

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
    debug("Pipeline layout created");
}

void setPipelineDetails() {
    const uint32_t minUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
    const uint32_t maxUniformBufferRange = umin(physicalDeviceProperties.limits.maxUniformBufferRange, USHRT_MAX + 1);

    cameraUniformAlignment    = align(sizeof(CameraUniform),    minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), minUniformBufferOffsetAlignment);
    materialUniformAlignment  = align(sizeof(MaterialUniform),  minUniformBufferOffsetAlignment);

    debug("Uniform Alignments:");
    debug("\tCamera:    %u", cameraUniformAlignment);
    debug("\tPrimitive: %u", primitiveUniformAlignment);
    debug("\tMaterial:  %u", materialUniformAlignment);

    lightingUniformBufferRange = ulmin(sizeof(LightingUniform), maxUniformBufferRange);

    cameraUniformBufferRange    = alignBack(maxUniformBufferRange, cameraUniformAlignment);
    primitiveUniformBufferRange = alignBack(maxUniformBufferRange, primitiveUniformAlignment);
    materialUniformBufferRange  = alignBack(maxUniformBufferRange, materialUniformAlignment);

    debug("Uniform Buffer Ranges:");
    debug("\tLighting:  %u", lightingUniformBufferRange);
    debug("\tCamera:    %u", cameraUniformBufferRange);
    debug("\tPrimitive: %u", primitiveUniformBufferRange);
    debug("\tMaterial:  %u", materialUniformBufferRange);

    pointLightCountLimit = umin(LIGHT_COUNT_HARD_LIMIT, lightingUniformBufferRange / sizeof(PointLightUniform));

    cameraCountLimit    = cameraUniformBufferRange    / cameraUniformAlignment;
    primitiveCountLimit = primitiveUniformBufferRange / primitiveUniformAlignment;
    materialCountLimit  = materialUniformBufferRange  / materialUniformAlignment;

    nodeCountLimit = primitiveCountLimit;

    debug("Count Limits:");
    debug("\tPoint Lights:");
    debug("\t\tHard Limit: %u", LIGHT_COUNT_HARD_LIMIT);
    debug("\t\tSoft Limit: %u", pointLightCountLimit);
    debug("\tCameras:      %u", cameraCountLimit);
    debug("\tPrimitives:   %u", primitiveCountLimit);
    debug("\tMaterials:    %u", materialCountLimit);
    debug("\tNodes:        %u", nodeCountLimit);

    framebufferUniformBufferSize    = lightingUniformBufferRange + cameraUniformBufferRange + primitiveUniformBufferRange + materialUniformBufferRange;
    framebufferSetUniformBufferSize = framebufferSetFramebufferCountLimit * framebufferUniformBufferSize;
}

void createPipeline() {
    createSampler();

    uint32_t framebufferCountLimit = framebufferSetCountLimit * framebufferSetFramebufferCountLimit;

    createDescriptorPool(&lightingDescriptorPool,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferCountLimit, VK_SHADER_STAGE_FRAGMENT_BIT, 4 /* Ambient, Point, Spot, Directional */);
    createDescriptorPool(&cameraDescriptorPool,    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1);
    createDescriptorPool(&primitiveDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit, VK_SHADER_STAGE_VERTEX_BIT,   1);
    createDescriptorPool(&materialDescriptorPool,  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
    createDescriptorPool(&samplerDescriptorPool,   VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCountLimit,    VK_SHADER_STAGE_FRAGMENT_BIT, materialTextureCount);
    createDescriptorPool(&storageDescriptorPool,   VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1,                     VK_SHADER_STAGE_VERTEX_BIT,   2 /* Index, Vertex */);

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

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadDeviceFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(framebuffer->renderCommandBuffer, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE);

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 5, 1, &storageDescriptorSet, 0, nullptr);
}

void destroyPipeline() {
    destroyDescriptorPool(&storageDescriptorPool);
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
