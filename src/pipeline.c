#include "pipeline.h"

#include "helper.h"

#include "buffer.h"
#include "content.h"

extern VkDevice device;

extern Buffer sharedBuffer;

extern size_t materialCountLimit;

VkDescriptorPool descriptorPool;
VkDescriptorSetLayout descriptorSetLayout;

VkPipelineLayout pipelineLayout;

void createLayouts() {
    VkDescriptorSetLayoutBinding layoutBindings[] = {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        },{
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = NULL
        }
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = sizeof(layoutBindings) / sizeof(VkDescriptorSetLayoutBinding),
        .pBindings = layoutBindings
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout);
    debug("Descriptor set layout created");

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

void createDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1
        }, {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1
        }
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = materialCountLimit,
        .poolSizeCount = sizeof(poolSizes) / sizeof(VkDescriptorPoolSize),
        .pPoolSizes = poolSizes
    };

    vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool);
    debug("Descriptor pool created");
}

void destroyPipeline() {
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    debug("Pipeline layout destroyed");

    vkDestroyDescriptorPool(device, descriptorPool, NULL);
    debug("Descriptor pool destroyed");

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, NULL);
    debug("Descriptor set layout destroyed");
}
