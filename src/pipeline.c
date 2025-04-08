#include "pipeline.h"

#include "device.h"
#include "material.h"
#include "primitive.h"
#include "framebuffer.h"
#include "descriptor.h"

#include "logger.h"

VkPipelineLayout pipelineLayout;

VkSampler sampler;

void createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL
    };

    vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout);
    debug("Pipeline layout created");
}

void createSampler(uint32_t mipLevelLimit, VkSampler *outSampler) {
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.0f,
        .maxLod = mipLevelLimit,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(device, &samplerInfo, NULL, outSampler);
    debug("Texture sampler created");
}

void createPipeline() {
    createDescriptorSetLayout();
    createPipelineLayout();

    createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferCountLimit, &sceneDescriptorPool);
    createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit, &primitiveDescriptorPool);
    createDescriptorPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCountLimit,    &materialDescriptorPool);

    const uint32_t mipLevelLimit = floor(log2(textureSizeMaxDimensionLimit)) + 1;
    createSampler( mipLevelLimit, &sampler);
}

void destroyPipeline() {
    vkDestroySampler(device, sampler, NULL);
    debug("Texture sampler destroyed");

    vkDestroyDescriptorPool(device, materialDescriptorPool,  NULL);
    vkDestroyDescriptorPool(device, primitiveDescriptorPool, NULL);
    vkDestroyDescriptorPool(device, sceneDescriptorPool,     NULL);
    debug("Descriptor pools destroyed");

    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    debug("Pipeline layout destroyed");

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");
}
