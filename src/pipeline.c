#include "pipeline.h"

#include "physicalDevice.h"
#include "device.h"
#include "material.h"
#include "primitive.h"
#include "framebuffer.h"
#include "descriptor.h"

#include "numerics.h"
#include "logger.h"

VkPipelineLayout pipelineLayout;

VkDeviceSize sceneUniformAlignment;
VkDeviceSize primitiveUniformAlignment;
VkDeviceSize dynamicUniformBufferRange;
VkDeviceSize framebufferUniformStride;

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
    sceneUniformAlignment     = align(sizeof(SceneUniform),     physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    primitiveUniformAlignment = align(sizeof(PrimitiveUniform), physicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    dynamicUniformBufferRange = alignBack(umin(physicalDeviceProperties.limits.maxUniformBufferRange, USHRT_MAX + 1), primitiveUniformAlignment);
    framebufferUniformStride  = sceneUniformAlignment + dynamicUniformBufferRange;

    createDescriptorSetLayout();
    createPipelineLayout();

    createSampler();

    sceneDescriptorPool     = createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         framebufferCountLimit);
    primitiveDescriptorPool = createDescriptorPool(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, framebufferCountLimit);
    materialDescriptorPool  = createDescriptorPool(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, materialCountLimit);
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
