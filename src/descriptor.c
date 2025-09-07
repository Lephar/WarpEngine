#include "descriptor.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "content.h"

#include "logger.h"

VkSampler sampler;

DescriptorPool sceneDescriptorPool;
DescriptorPool primitiveDescriptorPool;
DescriptorPool factorDescriptorPool;
DescriptorPool materialDescriptorPool;

// TODO: Load sampler from asset file
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
        .borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(device, &samplerInfo, NULL, &sampler);
    debug("Texture sampler created");
}

void createBufferDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage) {
    debug("%s buffer descriptor pool:", descriptorPool->type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ? "Dynamic uniform" : "Uniform");

    descriptorPool->type  = type;
    descriptorPool->count = count;
    descriptorPool->stage = stage;

    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = 0,
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
    debug("\tDescriptor set layout created");

    VkDescriptorPoolSize poolSize = {
        .type = type,
        .descriptorCount = count
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
    debug("\tDescriptor pool created with %u sets", descriptorPool->count);
}

void createSamplerDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t count, VkShaderStageFlags stage) {
    debug("Combined image sampler descriptor pool:");

    descriptorPool->type  = type;
    descriptorPool->count = count;
    descriptorPool->stage = stage;

    VkDescriptorSetLayoutBinding *layoutBindings = malloc(materialTextureCount * sizeof(VkDescriptorSetLayoutBinding));

    for(uint32_t bindingIndex = 0; bindingIndex < materialTextureCount; bindingIndex++) {
        VkDescriptorSetLayoutBinding *layoutBinding = &layoutBindings[bindingIndex];

        layoutBinding->binding = bindingIndex;
        layoutBinding->descriptorType = type;
        layoutBinding->descriptorCount = 1;
        layoutBinding->stageFlags = stage;
        layoutBinding->pImmutableSamplers = NULL;
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = materialTextureCount,
        .pBindings = layoutBindings
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorPool->layout);
    debug("\tDescriptor set layout created with %u bindings", materialTextureCount);

    VkDescriptorPoolSize poolSize = {
        .type = type,
        .descriptorCount = count
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
    debug("\tDescriptor pool created with %u sets", descriptorPool->count);
}

// TODO: Count the allocated sets
VkDescriptorSet allocateDescriptorSet(DescriptorPool *descriptorPool) {
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool->pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorPool->layout
    };

    vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);

    return descriptorSet;
}

VkDescriptorSet createBufferDescriptorSet(DescriptorPool *descriptorPool, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);

    VkDescriptorBufferInfo bufferInfo = {
        .buffer = buffer,
        .offset = offset,
        .range  = range
    };

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = NULL,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = descriptorPool->type,
        .pImageInfo = NULL,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = NULL
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);

    return descriptorSet;
}

VkDescriptorSet createImageDescriptorSet(DescriptorPool *descriptorPool, VkSampler sampler, Material *material) {
    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);

    VkDescriptorImageInfo imageInfos[] = {
        {
            .sampler     = sampler,
            .imageView   = material->baseColor->view,
            .imageLayout = material->baseColor->layout
        }, {
            .sampler     = sampler,
            .imageView   = material->metallicRoughness->view,
            .imageLayout = material->metallicRoughness->layout
        }, {
            .sampler     = sampler,
            .imageView   = material->normal->view,
            .imageLayout = material->normal->layout
        }
    };

    VkWriteDescriptorSet descriptorWrites[materialTextureCount];

    for(uint32_t bindingIndex = 0; bindingIndex < materialTextureCount; bindingIndex++) {
        VkDescriptorImageInfo *imageInfo = &imageInfos[bindingIndex];
        VkWriteDescriptorSet *descriptorWrite = &descriptorWrites[bindingIndex];

        descriptorWrite->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite->pNext = NULL;
        descriptorWrite->dstSet = descriptorSet;
        descriptorWrite->dstBinding = bindingIndex;
        descriptorWrite->dstArrayElement = 0;
        descriptorWrite->descriptorCount = 1;
        descriptorWrite->descriptorType = descriptorPool->type;
        descriptorWrite->pImageInfo = imageInfo;
        descriptorWrite->pBufferInfo = NULL;
        descriptorWrite->pTexelBufferView = NULL;
    }

    vkUpdateDescriptorSets(device, materialTextureCount, descriptorWrites, 0, NULL);

    return descriptorSet;
}

VkDescriptorSet getSceneDescriptorSet(uint32_t framebufferIndex) {
    return createBufferDescriptorSet(&sceneDescriptorPool, sharedBuffer.buffer, framebufferSetUniformBufferOffset + framebufferIndex * framebufferUniformBufferStride + primitiveUniformBufferRange, sceneUniformAlignment);
}

VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferIndex) {
    return createBufferDescriptorSet(&primitiveDescriptorPool, sharedBuffer.buffer, framebufferSetUniformBufferOffset + framebufferIndex * framebufferUniformBufferStride, primitiveUniformBufferRange);
}

VkDescriptorSet getFactorDescriptorSet() {
    return createBufferDescriptorSet(&factorDescriptorPool, sharedBuffer.buffer, 0, materialUniformBufferRange);
}

VkDescriptorSet getMaterialDescriptorSet(Material *material) {
    return createImageDescriptorSet(&materialDescriptorPool, sampler, material);
}

void resetDescriptorPool(DescriptorPool *descriptorPool) {
    vkResetDescriptorPool(device, descriptorPool->pool, 0);
}

void destroyDescriptorPool(DescriptorPool *descriptorPool) {
    vkDestroyDescriptorPool(device, descriptorPool->pool, NULL);
    vkDestroyDescriptorSetLayout(device, descriptorPool->layout, NULL);
}
