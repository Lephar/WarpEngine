#include "material.h"

#include "config.h"
#include "physicalDevice.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "pipeline.h"
#include "descriptor.h"
#include "content.h"

#include "file.h"
#include "logger.h"
#include "numerics.h"

Image *defaultBlackTexture = NULL;
Image *defaultWhiteTexture = NULL;

const uint32_t materialTextureCount = 3;
uint32_t materialCountLimit;
uint32_t materialCount;
Material *materials;
MaterialUniform *materialUniforms;

VkDescriptorSet factorDescriptorSet;

uint32_t findMaterial(cgltf_material *materialData) {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(materialData->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            return materialIndex;
        }
    }

    return UINT32_MAX;
}

Image *loadUncompressedTexture(const char *subdirectory, const char *filename, bool isColor) {
    char path[PATH_MAX];
    makeFullPath(subdirectory, filename, path);
    debug("\tImage Path: %s", path);

    int32_t width  = 0;
    int32_t height = 0;
    int32_t depth  = STBI_rgb_alpha; // TODO: Consider the normal and metallic roughness channels

    // NOTICE: Allocates data double the necessary size because of our STBI_MALLOC override in implementation.c
    uint8_t *data = stbi_load(path, &width, &height, NULL, depth);

    assert((uint32_t) width <= physicalDeviceProperties.limits.maxImageDimension2D && (uint32_t) height <= physicalDeviceProperties.limits.maxImageDimension2D);

    size_t   size = width * height * depth;
    uint32_t mips = floor(log2(imax(width, height))) + 1;

    debug("\t\tWidth:  %u", width);
    debug("\t\tHeight: %u", height);
    debug("\t\tDepth:  %u", depth);
    debug("\t\tMips:   %u", mips);

    ktx_error_code_e result;
    ktxTexture2 *compressedTexture;
    ktxTexture *compressedTextureHandle;

    ktxTextureCreateInfo compressedTextureCreateInfo = {
        .glInternalformat = 0, // Ignored
        .vkFormat = isColor ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        .pDfd = NULL, // Ignored
        .baseWidth = width,
        .baseHeight = height,
        .baseDepth = 1,
        .numDimensions = 2,
        .numLevels = mips,
        .numLayers = 1,
        .numFaces = 1,
        .isArray = KTX_FALSE,
        .generateMipmaps = KTX_FALSE
    };

    result = ktxTexture2_Create(&compressedTextureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &compressedTexture);

    if(result != KTX_SUCCESS) {
        debug("\t\tCreating texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    compressedTextureHandle = (ktxTexture *) compressedTexture;

    debug("\t\tRaw Size:   %lu", size);
    debug("\t\tFinal Size: %lu", ktxTexture_GetDataSize(compressedTextureHandle));

    uint32_t srcWidth  = width;
    uint32_t srcHeight = height;
    size_t   srcOffset = 0;
    size_t   srcSize   = size;

    result = ktxTexture_SetImageFromMemory(compressedTextureHandle, 0, 0, 0, data + srcOffset, srcSize);

    if(result != KTX_SUCCESS) {
        debug("\t\tSetting texture from memory failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    for(uint32_t level = 1; level < mips; level++) {
        uint32_t dstWidth  = umax(srcWidth  / 2, 1);
        uint32_t dstHeight = umax(srcHeight / 2, 1);
        size_t   dstOffset = srcOffset + srcSize;
        size_t   dstSize   = dstWidth * dstHeight * depth;
        /*
        debug("\t\tLevel: %u", level);
        debug("\t\t\tWidth:  %u", dstWidth);
        debug("\t\t\tHeight: %u", dstHeight);
        debug("\t\t\tOffset: %u", dstOffset);
        debug("\t\t\tSize:   %u", dstSize);
        */
        stbir_resize_uint8_srgb(data + srcOffset, srcWidth, srcHeight, 0, data + dstOffset, dstWidth, dstHeight, 0, STBIR_RGBA);

        result = ktxTexture_SetImageFromMemory(compressedTextureHandle, level, 0, 0, data + dstOffset, dstSize);

        if(result != KTX_SUCCESS) {
            debug("\t\tSetting level from memory failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        srcWidth  = dstWidth;
        srcHeight = dstHeight;
        srcOffset = dstOffset;
        srcSize   = dstSize;
    }

    stbi_image_free(data);

    ktxBasisParams compressionParameters = {
        .structSize = sizeof(ktxBasisParams),
        .uastc = KTX_TRUE, // TODO: Dive further into that compression optimization rabbit hole
        .threadCount = threadCount,
        .compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL,
        .normalMap = false // TODO: Research that topic to see how that can be used for metallic roughness and normal maps
    }; // NOTICE: Many more params exist here that are zero initialized here

    result = ktxTexture2_CompressBasisEx(compressedTexture, &compressionParameters);

    if(result != KTX_SUCCESS) {
        debug("\t\tCompressing texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    debug("\t\tCompressed Size: %lu", ktxTexture_GetDataSize(compressedTextureHandle));

    if(ktxTexture2_NeedsTranscoding(compressedTexture)) {
        result = ktxTexture2_TranscodeBasis(compressedTexture, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        debug("\t\tTranscoded Size: %lu", ktxTexture_GetDataSize(compressedTextureHandle));
    }

    Image *texture = createImage(width, height, mips, VK_SAMPLE_COUNT_1_BIT, isColor ? VK_FORMAT_BC7_SRGB_BLOCK : VK_FORMAT_BC7_UNORM_BLOCK, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);
    bindImageMemory(texture, &deviceMemory);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    debug("\t\tAligned Size:    %lu", texture->memoryRequirements.size);
    debug("\t\tMemory Offset:   %lu", texture->memoryOffset);

    data = ktxTexture_GetData(compressedTextureHandle);

    for(ktx_uint32_t level = 0; level < mips; level++) {
        result = ktxTexture2_GetImageOffset(compressedTexture, level, 0, 0, &srcOffset);

        if(result != KTX_SUCCESS) {
            debug("\t\tGetting mip level %d data failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        srcSize = ktxTexture_GetImageSize(compressedTextureHandle, level);

        if(srcSize <= sharedBuffer.size) {
            memcpy(mappedSharedMemory, data + srcOffset, srcSize);
            copyBufferToImage(&sharedBuffer, 0, texture, level);
        } else if (srcSize <= deviceBuffer.size) {
            stagingBufferCopy(data, srcOffset, 0, srcSize);
            copyBufferToImage(&deviceBuffer, 0, texture, level);
        } else {
            debug("\t\tCan't copy image data, increase shared or device local buffer size!");
            assert(srcSize <= sharedBuffer.size || srcSize <= deviceBuffer.size);
        }
    }

    ktxTexture2_Destroy(compressedTexture);

    transitionImageLayout(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    createImageView(texture);

    debug("\t\tTexture created");

    return texture;
}

Image *loadTexture(const char *subdirectory, const char *filename, bool isColor) {
    char path[PATH_MAX];
    makeFullPath(subdirectory, filename, path);
    debug("\tImage Path: %s", path);

    ktxTexture2 *textureObject;
    KTX_error_code result;

    result = ktxTexture2_CreateFromNamedFile(path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &textureObject);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    ktx_uint32_t width  = textureObject->baseWidth;
    ktx_uint32_t height = textureObject->baseHeight;
    ktx_uint32_t depth  = textureObject->baseDepth;
    ktx_uint32_t mips   = textureObject->numLevels;

    assert(width <= physicalDeviceProperties.limits.maxImageDimension2D && height <= physicalDeviceProperties.limits.maxImageDimension2D);

    debug("\t\tWidth:  %u", width);
    debug("\t\tHeight: %u", height);
    debug("\t\tDepth:  %u", depth);
    debug("\t\tMips:   %u", mips);

    // TODO: Most ktxTexture2_* functions exist in the docs but aren't really exposed. Remove this old handle when they update.
    ktxTexture *textureObjectHandle = (ktxTexture *) textureObject;
    debug("\t\tCompressed Size: %lu", ktxTexture_GetDataSize(textureObjectHandle));

    if(ktxTexture2_NeedsTranscoding(textureObject)) {
        result = ktxTexture2_TranscodeBasis(textureObject, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        debug("\t\tTranscoded Size: %lu", ktxTexture_GetDataSize(textureObjectHandle));
    }

    Image *texture = createImage(width, height, mips, VK_SAMPLE_COUNT_1_BIT, isColor ? VK_FORMAT_BC7_SRGB_BLOCK : VK_FORMAT_BC7_UNORM_BLOCK, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);
    bindImageMemory(texture, &deviceMemory);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    debug("\t\tAligned Size:    %lu", texture->memoryRequirements.size);
    debug("\t\tMemory Offset:   %lu", texture->memoryOffset);

    ktx_uint8_t *data = ktxTexture_GetData(textureObjectHandle);

    for(ktx_uint32_t level = 0; level < mips; level++) {
        ktx_size_t offset;
        result = ktxTexture2_GetImageOffset(textureObject, level, 0, 0, &offset);

        if(result != KTX_SUCCESS) {
            debug("\t\tGetting mip level %d data failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        ktx_size_t size = ktxTexture_GetImageSize(textureObjectHandle, level);

        if(size <= sharedBuffer.size) {
            memcpy(mappedSharedMemory, data + offset, size);
            copyBufferToImage(&sharedBuffer, 0, texture, level);
        } else if (size <= deviceBuffer.size) {
            stagingBufferCopy(data, offset, 0, size);
            copyBufferToImage(&deviceBuffer, 0, texture, level);
        } else {
            debug("\t\tCan't copy image data, increase shared or device local buffer size!");
            assert(size <= sharedBuffer.size || size <= deviceBuffer.size);
        }
    }

    ktxTexture2_Destroy(textureObject);

    transitionImageLayout(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    createImageView(texture);

    debug("\t\tTexture created");

    return texture;
}

void loadMaterial(const char *subdirectory,cgltf_material *materialData) {
    assert(materialCount < materialCountLimit);
    const uint32_t materialIndex = materialCount;

    Material        *material        = &materials[materialIndex];
    MaterialUniform *materialUniform = &materialUniforms[materialIndex];

    debug("Material Name: %s", materialData->name);
    strncpy(material->name, materialData->name, UINT8_MAX);

    material->baseColor         = NULL;
    material->metallicRoughness = NULL;
    material->normal            = NULL;

    if(materialData->alpha_mode == cgltf_alpha_mode_blend) {
        material->isTransparent = true;
    } else {
        material->isTransparent = false;
    }

    material->isDoubleSided = materialData->double_sided;

    if(materialData->has_pbr_metallic_roughness) {
        memcpy(materialUniform->baseColorFactor, materialData->pbr_metallic_roughness.base_color_factor, sizeof(vec4));
        debug("\tBase color factor: [%0.4f, %0.4f, %0.4f, %0.4f]", materialUniform->baseColorFactor[0], materialUniform->baseColorFactor[1], materialUniform->baseColorFactor[2], materialUniform->baseColorFactor[3]);

        if(materialData->pbr_metallic_roughness.base_color_texture.texture) {
            if(materialData->pbr_metallic_roughness.base_color_texture.texture->has_basisu) {
                material->baseColor = loadTexture(subdirectory, materialData->pbr_metallic_roughness.base_color_texture.texture->basisu_image->uri, true);
            } else {
                material->baseColor = loadUncompressedTexture(subdirectory, materialData->pbr_metallic_roughness.base_color_texture.texture->image->uri, true);
            }
        } else {
            material->baseColor = loadUncompressedTexture("assets/default/textures", "white.png", true);
        }

        materialUniform->metallicRoughnessFactor[0] = materialData->pbr_metallic_roughness.metallic_factor;
        materialUniform->metallicRoughnessFactor[1] = materialData->pbr_metallic_roughness.roughness_factor;
        debug("\tMetallic roughness factor: [%0.4f, %0.4f]", materialUniform->metallicRoughnessFactor[0], materialUniform->metallicRoughnessFactor[1]);

        if(materialData->pbr_metallic_roughness.metallic_roughness_texture.texture) {
            if(materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->has_basisu) {
                material->metallicRoughness = loadTexture(subdirectory, materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->basisu_image->uri, false);
            } else {
                material->metallicRoughness = loadUncompressedTexture(subdirectory, materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri, false);
            }
        } else {
            material->metallicRoughness = loadUncompressedTexture("assets/default/textures", "black.png", false);
        }
    }

    if(materialData->normal_texture.texture) {
        materialUniform->normalScale = materialData->normal_texture.scale;
        debug("\tNormal scale: %0.4f", materialUniform->normalScale);

        if(materialData->normal_texture.texture->has_basisu) {
            material->normal = loadTexture(subdirectory, materialData->normal_texture.texture->basisu_image->uri, false);
        } else {
            material->normal = loadUncompressedTexture(subdirectory, materialData->normal_texture.texture->image->uri, false);
        }
    } else {
        material->normal = loadUncompressedTexture("assets/default/textures", "black.png", false);
    }

    memcpy(materialUniform->emissiveFactor, materialData->emissive_factor, sizeof(vec3));
    debug("\tEmissive factor: [%0.4f, %0.4f, %0.4f]", materialUniform->emissiveFactor[0], materialUniform->emissiveFactor[1], materialUniform->emissiveFactor[2]);

    material->factorOffset = materialIndex * materialUniformAlignment;
    material->materialDescriptorSet = getMaterialDescriptorSet(material);

    debug("\tTexture descriptor sets acquired and material created");

    materialCount++;
}

// NOTICE: This doesn't account for shader binding, use bindShader() beforehand
void bindMaterial(VkCommandBuffer commandBuffer, Material *material) {
    VkDescriptorSet descriptorSets[] = {
        material->materialDescriptorSet,
        factorDescriptorSet
    };

    uint32_t descriptorSetCount = sizeof(descriptorSets) / sizeof(VkDescriptorSet);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, descriptorSetCount, descriptorSets, 1, &material->factorOffset);
}

void destroyMaterial(Material *material) {
    if(material->normal) {
        destroyImageView(material->normal);
        destroyImage(material->normal);
    }

    if(material->metallicRoughness) {
        destroyImageView(material->metallicRoughness);
        destroyImage(material->metallicRoughness);
    }

    destroyImageView(material->baseColor);
    destroyImage(material->baseColor);
}
