#include "pipeline.h"

#include "device.h"
#include "material.h"
#include "primitive.h"
#include "framebuffer.h"
#include "descriptor.h"

#include "logger.h"

VkPipelineLayout pipelineLayout;

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
