#include "material.h"

#include "device.h"
#include "physicalDevice.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "pipeline.h"
#include "content.h"

#include "file.h"
#include "logger.h"

const uint32_t textureSizeMaxDimensionLimit = 8192;
const uint32_t materialCountLimit = 128;
uint32_t materialCount;
Material *materials;

Image *loadTexture(const char *path) {
    debug("\tImage Path: %s", path);

    ktxTexture2 *textureObject;
    KTX_error_code ktxResult;

    ktxResult = ktxTexture2_CreateFromNamedFile(path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &textureObject);

    if(ktxResult != KTX_SUCCESS) {
        debug("\tLoading texture failed with message: %s", ktxErrorString(ktxResult));
        assert(ktxResult == KTX_SUCCESS);
    }

    ktxTexture *textureHandle = (ktxTexture *) textureObject;
    debug("\tCompressed Size: %lu", ktxTexture_GetDataSize(textureHandle));

    if(ktxTexture2_NeedsTranscoding(textureObject)) {
        ktxResult = ktxTexture2_TranscodeBasis(textureObject, KTX_TTF_BC7_RGBA, 0);

        if(ktxResult != KTX_SUCCESS) {
            debug("\tTranscoding texture failed with message: %s", ktxErrorString(ktxResult));
            assert(ktxResult == KTX_SUCCESS);
        }

        debug("\tTranscoded Size: %lu", ktxTexture_GetDataSize(textureHandle));
    }

    debug("\tMemory Offset:   %lu", deviceMemory.offset);

    ktx_uint32_t width  = textureObject->baseWidth;
    ktx_uint32_t height = textureObject->baseHeight;
    ktx_uint32_t depth  = textureObject->baseDepth;
    ktx_uint32_t mips   = textureObject->numLevels;

    debug("\tWidth:  %u", width);
    debug("\tHeight: %u", height);
    debug("\tDepth:  %u", depth);
    debug("\tMips:   %u", mips);

    assert(width <= textureSizeMaxDimensionLimit && height <= textureSizeMaxDimensionLimit);

    Image *texture = createImage(width, height, mips, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_BC7_SRGB_BLOCK, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    bindImageMemory(texture, &deviceMemory);
    createImageView(texture);

    debug("\tImage created");

    transitionImageLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    ktx_uint8_t *data = ktxTexture_GetData(textureHandle);

    for(ktx_uint32_t level = 0; level < mips; level++) {
        ktx_size_t offset;
        ktxResult = ktxTexture_GetImageOffset(textureHandle, level, 0, 0, &offset);

        if(ktxResult != KTX_SUCCESS) {
            debug("\tGetting mip level %d data failed with message: %s", level, ktxErrorString(ktxResult));
            assert(ktxResult == KTX_SUCCESS);
        }

        ktx_size_t levelSize = ktxTexture_GetImageSize(textureHandle, level);

        if(levelSize <= sharedBuffer.size) {
            memcpy(mappedSharedMemory, data + offset, levelSize);
            copyBufferToImage(&sharedBuffer, 0, texture, level);
        } else if (levelSize <= deviceBuffer.size) {
            stagingBufferCopy(data, offset, 0, levelSize);
            copyBufferToImage(&deviceBuffer, 0, texture, level);
        } else {
            debug("\tCan't copy image data, increase shared or device local buffer size!");
            assert(levelSize <= sharedBuffer.size || levelSize <= deviceBuffer.size);
        }
    }

    debug("\tImage data copied");

    transitionImageLayout(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    ktxTexture_Destroy(textureHandle);

    return texture;
}

void createDescriptor(Material *material) {
    VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    vkAllocateDescriptorSets(device, &allocateInfo, &material->samplerDescriptor);

    VkDescriptorBufferInfo bufferInfo = {
        .buffer = sharedBuffer.buffer,
        .offset = 0,
        .range = physicalDeviceProperties.limits.maxUniformBufferRange
    };

    VkDescriptorImageInfo imageInfo = {
        .sampler = sampler,
        .imageView = material->baseColor->view,
        .imageLayout = material->baseColor->layout
    };

    VkWriteDescriptorSet descriptorWrites[] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = material->samplerDescriptor,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = NULL
        }, {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = material->samplerDescriptor,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL
        }
    };

    vkUpdateDescriptorSets(device, sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet), descriptorWrites, 0, NULL);
    debug("\tDescriptor created");
}

void loadMaterial(Material *material, cgltf_material *materialData) {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(materialData->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            return;
        }
    }

    debug("Material Name: %s", materialData->name);
    strncpy(material->name, materialData->name, UINT8_MAX);
    assert(materialData->has_pbr_metallic_roughness && materialData->pbr_metallic_roughness.base_color_texture.texture->has_basisu);

    char textureFullPath[PATH_MAX];
    makeFullPath("data", materialData->pbr_metallic_roughness.base_color_texture.texture->basisu_image->uri, textureFullPath);

    material->baseColor = loadTexture(textureFullPath);
    createDescriptor(material);
}

void destroyMaterial(Material *material) {
    destroyImageView(material->baseColor);
    destroyImage(material->baseColor);
    material->baseColor = NULL;
}
