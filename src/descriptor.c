#include "descriptor.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "image.h"
#include "content.h"

#include "logger.h"

VkSampler sampler;

DescriptorPool sceneDescriptorPool;
DescriptorPool primitiveDescriptorPool;
DescriptorPool materialDescriptorPool;

void createSampler() {
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
        .maxLod = floor(log2(physicalDeviceProperties.limits.maxImageDimension2D)) + 1,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(device, &samplerInfo, NULL, &sampler);
    debug("Texture sampler created");
}

void createDescriptorPool(DescriptorPool *descriptorPool, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage) {
    descriptorPool->binding = binding;
    descriptorPool->type    = type;
    descriptorPool->count   = count;
    descriptorPool->stage   = stage;

    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = binding,
        .descriptorType = type,
            .descriptorCount = 1,
        .stageFlags = stage,
            .pImmutableSamplers = NULL
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &layoutBinding
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorPool->layout);
    debug("Descriptor set layout created");

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

    vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool->pool);
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

void makeBufferDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorType type, uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
    VkDescriptorBufferInfo bufferInfo = {
        .buffer = buffer,
        .offset = offset,
        .range  = range
    };

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = NULL,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = NULL,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = NULL
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
    debug("\tDescriptor created");
}

void makeImageDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorType type, uint32_t binding, VkSampler sampler, Image *image) {
    VkDescriptorImageInfo imageInfo = {
        .sampler     = sampler,
        .imageView   = image->view,
        .imageLayout = image->layout
    };

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = NULL,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = &imageInfo,
        .pBufferInfo = NULL,
        .pTexelBufferView = NULL
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
    debug("\tDescriptor created");
}

VkDescriptorSet getSceneDescriptorSet(uint32_t index) {
    VkDescriptorSet descriptorSet = allocateDescriptorSet(sceneDescriptorPool);
    makeBufferDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, sharedBuffer.buffer, index * framebufferUniformStride, sceneUniformAlignment);
    return descriptorSet;
}

VkDescriptorSet getPrimitiveDescriptorSet(uint32_t index) {
    VkDescriptorSet descriptorSet = allocateDescriptorSet(primitiveDescriptorPool);
    makeBufferDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, sharedBuffer.buffer, index * framebufferUniformStride + sceneUniformAlignment, dynamicUniformBufferRange);
    return descriptorSet;
}

VkDescriptorSet getMaterialDescriptorSet(Image *image) {
    VkDescriptorSet descriptorSet = allocateDescriptorSet(materialDescriptorPool);
    makeImageDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2, sampler, image);
    return descriptorSet;
}
