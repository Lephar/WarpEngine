#include "descriptor.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "content.h"

#include "logger.h"

DescriptorPool   storageDescriptorPool;
DescriptorPool primitiveDescriptorPool;
DescriptorPool    cameraDescriptorPool;
DescriptorPool  lightingDescriptorPool;
DescriptorPool  materialDescriptorPool;
DescriptorPool   samplerDescriptorPool;

const PDescriptorPool descriptorPoolReferences[] = {
      &storageDescriptorPool,
    &primitiveDescriptorPool,
       &cameraDescriptorPool,
     &lightingDescriptorPool,
     &materialDescriptorPool,
      &samplerDescriptorPool,
};

const uint32_t descriptorPoolCount = sizeof(descriptorPoolReferences) / sizeof(*descriptorPoolReferences);

VkSampler sampler;
VkDescriptorSet storageDescriptorSet;

// TODO: Load sampler from asset file
void createSampler() {
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
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
        .maxLod = (float) floor(log2(physicalDeviceProperties.limits.maxImageDimension2D)) + 1,
        .borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(device, &samplerInfo, nullptr, &sampler);
    debug("Texture sampler created");
}

void createDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, uint32_t descriptorCount, VkShaderStageFlags stage, uint32_t bindingCount) {
    debug("Descriptor pool:");

    descriptorPool->type            = type;
    descriptorPool->descriptorCount = descriptorCount;
    descriptorPool->stage           = stage;
    descriptorPool->bindingCount    = bindingCount;

    VkDescriptorSetLayoutBinding *layoutBindings = malloc(bindingCount * sizeof(VkDescriptorSetLayoutBinding));

    for(uint32_t binding = 0; binding < bindingCount; binding++)
    {
        VkDescriptorSetLayoutBinding *layoutBinding = &layoutBindings[binding];

        layoutBinding->binding = binding;
        layoutBinding->descriptorType = type;
        layoutBinding->descriptorCount = 1;
        layoutBinding->stageFlags = stage;
        layoutBinding->pImmutableSamplers = nullptr;
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = bindingCount,
        .pBindings = layoutBindings,
    };

    vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorPool->layout);
    free(layoutBindings);
    debug("\tDescriptor set layout created");

    VkDescriptorPoolSize poolSize = {
        .type = type,
        .descriptorCount = descriptorCount * bindingCount,
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = descriptorCount,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool->pool);
    debug("\tDescriptor pool created with %u sets and %u bindings", descriptorPool->descriptorCount, descriptorPool->bindingCount);
}

// TODO: Count the allocated sets
VkDescriptorSet allocateDescriptorSet(DescriptorPool *descriptorPool) {
    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool->pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorPool->layout
    };

    vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);

    return descriptorSet;
}

VkDescriptorSet createBufferDescriptorSet(DescriptorPool *descriptorPool, VkBuffer buffer, VkDeviceSize offsets[], VkDeviceSize ranges[]) {
    debug("Buffer Descriptor Set:");

    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);
    debug("\tAllocated");

    VkDescriptorBufferInfo *bufferInfos      = malloc(descriptorPool->bindingCount * sizeof(VkDescriptorBufferInfo));
    VkWriteDescriptorSet   *descriptorWrites = malloc(descriptorPool->bindingCount * sizeof(VkWriteDescriptorSet));

    for(uint32_t binding = 0; binding < descriptorPool->bindingCount; binding++) {
        VkDescriptorBufferInfo *bufferInfo      = &bufferInfos[binding];
        VkWriteDescriptorSet   *descriptorWrite = &descriptorWrites[binding];

        VkDeviceSize offset = offsets[binding];
        VkDeviceSize range  = ranges[binding];

        bufferInfo->buffer = buffer;
        bufferInfo->offset = offset;
        bufferInfo->range  = range;

        debug("\tBinding:  %u",  binding);
        debug("\t\tOffset: %lu", offset);
        debug("\t\tRange:  %lu", range);

        descriptorWrite->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite->pNext = nullptr;
        descriptorWrite->dstSet = descriptorSet;
        descriptorWrite->dstBinding = binding;
        descriptorWrite->dstArrayElement = 0;
        descriptorWrite->descriptorCount = 1;
        descriptorWrite->descriptorType = descriptorPool->type;
        descriptorWrite->pImageInfo = nullptr;
        descriptorWrite->pBufferInfo = bufferInfo;
        descriptorWrite->pTexelBufferView = nullptr;
    }

    vkUpdateDescriptorSets(device, descriptorPool->bindingCount, descriptorWrites, 0, nullptr);
    free(descriptorWrites);
    free(bufferInfos);
    debug("\tUpdated");

    return descriptorSet;
}

VkDescriptorSet createImageDescriptorSet(DescriptorPool *descriptorPool, Material *material) {
    assert(descriptorPool->bindingCount == materialTextureCount);

    debug("Sampler Descriptor Set:");

    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);
    debug("\tAllocated");

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
            .imageView   = material->emissive->view,
            .imageLayout = material->emissive->layout
        }, {
            .sampler     = sampler,
            .imageView   = material->occlusion->view,
            .imageLayout = material->occlusion->layout
        }, {
            .sampler     = sampler,
            .imageView   = material->normal->view,
            .imageLayout = material->normal->layout
        }
    };

    debug("\tBinding Count: %u", descriptorPool->bindingCount);

    VkWriteDescriptorSet *descriptorWrites = malloc(descriptorPool->bindingCount * sizeof(VkWriteDescriptorSet));

    for(uint32_t binding = 0; binding < descriptorPool->bindingCount; binding++) {
        VkWriteDescriptorSet *descriptorWrite = &descriptorWrites[binding];

        descriptorWrite->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite->pNext = nullptr;
        descriptorWrite->dstSet = descriptorSet;
        descriptorWrite->dstBinding = binding;
        descriptorWrite->dstArrayElement = 0;
        descriptorWrite->descriptorCount = 1;
        descriptorWrite->descriptorType = descriptorPool->type;
        descriptorWrite->pImageInfo = &imageInfos[binding];
        descriptorWrite->pBufferInfo = nullptr;
        descriptorWrite->pTexelBufferView = nullptr;
    }

    vkUpdateDescriptorSets(device, descriptorPool->bindingCount, descriptorWrites, 0, nullptr);
    free(descriptorWrites);
    debug("\tUpdated");

    return descriptorSet;
}

VkDescriptorSet getStorageDescriptorSet(VkDeviceSize offsets[], VkDeviceSize ranges[]) {
    return createBufferDescriptorSet(&storageDescriptorPool, deviceBuffer.buffer, offsets, ranges);
}

VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize offset = framebufferSetIndex * framebufferSetUniformBufferSize + framebufferIndex * framebufferUniformBufferSize + primitiveUniformBufferOffset;
    VkDeviceSize range  = primitiveUniformBufferRange;

    return createBufferDescriptorSet(&primitiveDescriptorPool, sharedBuffer.buffer, &offset, &range);
}

VkDescriptorSet getCameraDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize offset = framebufferSetIndex * framebufferSetUniformBufferSize + framebufferIndex * framebufferUniformBufferSize + cameraUniformBufferOffset;
    VkDeviceSize range  = cameraUniformBufferRange;

    return createBufferDescriptorSet(&cameraDescriptorPool, sharedBuffer.buffer, &offset, &range);
}

VkDescriptorSet getMaterialDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize offset = framebufferSetIndex * framebufferSetUniformBufferSize + framebufferIndex * framebufferUniformBufferSize + materialUniformBufferOffset;
    VkDeviceSize range  = materialUniformBufferRange;

    return createBufferDescriptorSet(&materialDescriptorPool,  sharedBuffer.buffer, &offset, &range);
}

VkDescriptorSet getSamplerDescriptorSet(Material *material) {
    return createImageDescriptorSet(&samplerDescriptorPool, material);
}

VkDescriptorSet getLightingDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize offset = framebufferSetIndex * framebufferSetUniformBufferSize + framebufferIndex * framebufferUniformBufferSize + sceneLightingUniformBufferOffset;

    VkDeviceSize offsets[] = {
        offset,
        offset + sceneLightingUniformBufferRange + lightUniformBufferRange * 0,
        offset + sceneLightingUniformBufferRange + lightUniformBufferRange * 1,
        offset + sceneLightingUniformBufferRange + lightUniformBufferRange * 2,
    };

    VkDeviceSize ranges[] = {
        sceneLightingUniformBufferRange,
        lightUniformBufferRange,
        lightUniformBufferRange,
        lightUniformBufferRange,
    };

    return createBufferDescriptorSet(&lightingDescriptorPool, sharedBuffer.buffer, offsets, ranges);
}

void resetDescriptorPool(DescriptorPool *descriptorPool) {
    vkResetDescriptorPool(device, descriptorPool->pool, 0);
}

void destroyDescriptorPool(DescriptorPool *descriptorPool) {
    vkDestroyDescriptorPool(device, descriptorPool->pool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorPool->layout, nullptr);
}
