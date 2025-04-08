#include "descriptor.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "logger.h"

VkDescriptorSetLayout descriptorSetLayout;

VkDescriptorPool sceneDescriptorPool;
VkDescriptorPool primitiveDescriptorPool;
VkDescriptorPool materialDescriptorPool;

void createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding layoutBindings[] = {
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = NULL
        },
        {
            .binding = 2,
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
}

void createDescriptorPool(VkDescriptorType type, uint32_t count, VkDescriptorPool *outDescriptorPool) {
    VkDescriptorPoolSize poolSize = {
        .type = type,
        .descriptorCount = 1
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = count,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };

    vkCreateDescriptorPool(device, &poolInfo, NULL, outDescriptorPool);
    debug("Descriptor pool created");
}

VkDescriptorSet allocateDescriptorSet(VkDescriptorPool descriptorPool) {
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);
    return descriptorSet;
}
