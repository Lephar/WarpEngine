#include "descriptor.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "image.h"
#include "light.h"
#include "material.h"
#include "content.h"
#include "framebuffer.h"

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

void createDescriptorPool(DescriptorPool *descriptorPool, VkDescriptorType type, VkShaderStageFlags stage, uint32_t setCount, uint32_t bindingCount) {
    debug("Descriptor pool:");

    descriptorPool->type            = type;
    descriptorPool->stage           = stage;
    descriptorPool->setIndex        = UINT32_MAX;
    descriptorPool->setCount        = setCount;
    descriptorPool->bindingCount    = bindingCount;
    descriptorPool->bindings        = nullptr;

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
        .descriptorCount = setCount * bindingCount,
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = setCount,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool->pool);
    debug("\tDescriptor pool created with %u sets and %u bindings", descriptorPool->setCount, descriptorPool->bindingCount);
}

void createDescriptorPools() {
    createDescriptorPool(&  storageDescriptorPool, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         VK_SHADER_STAGE_VERTEX_BIT,                                1,                     2 /* Index, Vertex */);
    createDescriptorPool(&primitiveDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT,                                framebufferCountLimit, 1);
    createDescriptorPool(&   cameraDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, framebufferCountLimit, 1);
    createDescriptorPool(& lightingDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         VK_SHADER_STAGE_FRAGMENT_BIT,                              framebufferCountLimit, 4 /* Point, Spot, Directional, Ambient */);
    createDescriptorPool(& materialDescriptorPool, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_FRAGMENT_BIT,                              framebufferCountLimit, 1);
    createDescriptorPool(&  samplerDescriptorPool, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,                              materialCountLimit,    materialTextureCount);

    for(uint32_t descriptorPoolIndex = 0; descriptorPoolIndex < descriptorPoolCount; descriptorPoolIndex++) {
        descriptorPoolReferences[descriptorPoolIndex]->setIndex = descriptorPoolIndex;
    }
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

VkDescriptorSet createBufferDescriptorSet(DescriptorPool *descriptorPool, VkDescriptorBufferInfo bufferInfos[]) {
    debug("Buffer Descriptor Set:");

    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);
    debug("\tAllocated");

    VkWriteDescriptorSet   *descriptorWrites = malloc(descriptorPool->bindingCount * sizeof(VkWriteDescriptorSet));

    for(uint32_t binding = 0; binding < descriptorPool->bindingCount; binding++) {
        VkDescriptorBufferInfo *bufferInfo      = &bufferInfos[binding];
        VkWriteDescriptorSet   *descriptorWrite = &descriptorWrites[binding];

        debug("\tBinding:  %u",  binding);
        debug("\t\tOffset: %lu", bufferInfo->offset);
        debug("\t\tRange:  %lu", bufferInfo->range);

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
    debug("\tUpdated");

    return descriptorSet;
}

VkDescriptorSet createImageDescriptorSet(DescriptorPool *descriptorPool, VkDescriptorImageInfo imageInfos[]) {
    assert(descriptorPool->bindingCount == materialTextureCount);

    debug("Sampler Descriptor Set:");

    VkDescriptorSet descriptorSet = allocateDescriptorSet(descriptorPool);
    debug("\tAllocated");

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

VkDescriptorSet getStorageDescriptorSet() {
    VkDescriptorBufferInfo bufferInfos[] = {
        {
            .buffer = deviceBuffer.buffer,
            .offset = 0,
            .range  = indexBufferSize,
        }, {
            .buffer = deviceBuffer.buffer,
            .offset = indexBufferSize,
            .range  = vertexBufferSize,
        },
    };

    return createBufferDescriptorSet(&storageDescriptorPool, bufferInfos);
}

VkDescriptorSet getPrimitiveDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize framebufferUniformBufferOffset = framebufferUniformBufferOffsets[framebufferSetIndex][framebufferIndex];

    VkDescriptorBufferInfo bufferInfos[] = {
        {
            .buffer = sharedBuffer.buffer,
            .offset = framebufferUniformBufferOffset + primitiveUniformBufferOffset,
            .range  = primitiveUniformBufferRange,
        },
    };

    return createBufferDescriptorSet(&primitiveDescriptorPool, bufferInfos);
}

VkDescriptorSet getCameraDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize framebufferUniformBufferOffset = framebufferUniformBufferOffsets[framebufferSetIndex][framebufferIndex];

    VkDescriptorBufferInfo bufferInfos[] = {
        {
            .buffer = sharedBuffer.buffer,
            .offset = framebufferUniformBufferOffset + cameraUniformBufferOffset,
            .range  = cameraUniformBufferRange,
        },
    };

    return createBufferDescriptorSet(&cameraDescriptorPool, bufferInfos);
}

VkDescriptorSet getLightingDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize framebufferUniformBufferOffset = framebufferUniformBufferOffsets[framebufferSetIndex][framebufferIndex];

    VkDescriptorBufferInfo bufferInfos[lightTypeCount];

    for(uint32_t lightTypeIndex = 0; lightTypeIndex < lightTypeCount; lightTypeIndex++) {
        VkDescriptorBufferInfo *bufferInfo = &bufferInfos[lightTypeIndex];

        bufferInfo->buffer = sharedBuffer.buffer;
        bufferInfo->offset = framebufferUniformBufferOffset + lightUniformBufferOffset + lightUniformBufferRange * lightTypeIndex;
        bufferInfo->range  = lightUniformBufferRange;
    }

    return createBufferDescriptorSet(&lightingDescriptorPool, bufferInfos);
}

VkDescriptorSet getMaterialDescriptorSet(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize framebufferUniformBufferOffset = framebufferUniformBufferOffsets[framebufferSetIndex][framebufferIndex];

    VkDescriptorBufferInfo bufferInfos[] = {
        {
            .buffer = sharedBuffer.buffer,
            .offset = framebufferUniformBufferOffset + materialUniformBufferOffset,
            .range  = materialUniformBufferRange,
        },
    };

    return createBufferDescriptorSet(&materialDescriptorPool, bufferInfos);
}

VkDescriptorSet getSamplerDescriptorSet(Material *material) {
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

    return createImageDescriptorSet(&samplerDescriptorPool, imageInfos);
}

void resetDescriptorPool(DescriptorPool *descriptorPool) {
    vkResetDescriptorPool(device, descriptorPool->pool, 0);
}

void destroyDescriptorPool(DescriptorPool *descriptorPool) {
    vkDestroyDescriptorPool(device, descriptorPool->pool, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorPool->layout, nullptr);
}
