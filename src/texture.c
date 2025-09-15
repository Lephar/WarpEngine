#include "texture.h"

#include "buffer.h"
#include "physicalDevice.h"
#include "memory.h"

#include "config.h"
#include "logger.h"
#include "file.h"
#include "image.h"
#include "numerics.h"

PRawTexture initializeRawTexture(const char *subdirectory, const char *filename, bool isColor) {
    PRawTexture texture = malloc(sizeof(RawTexture));

    makeFullPath(subdirectory, filename, texture->path);
    debug("\tImage Path: %s", texture->path);
    
    texture->isColor = isColor;
    texture->size    = 0;

    int32_t success = stbi_info(texture->path, (int32_t *) &texture->width, (int32_t *) &texture->height, (int32_t *) &texture->depth);

    if(success == false) {
        debug("\t\tImage meta data loading failed with message: %s", stbi_failure_reason());
        assert(success == true);
    }

    assert((uint32_t) texture->width <= physicalDeviceProperties.limits.maxImageDimension2D && (uint32_t) texture->height <= physicalDeviceProperties.limits.maxImageDimension2D);

    texture->mips = 0;
    texture->data = nullptr;

    debug("\t\tWidth:  %u", texture->width);
    debug("\t\tHeight: %u", texture->height);
    debug("\t\tOriginal Depth: %u", texture->depth);
    debug("\t\tRaw texture initialized");

    return texture;
}

void loadRawTexture(PRawTexture texture) {
    texture->data = stbi_load(texture->path, (int32_t *) &texture->width, (int32_t *) &texture->height, (int32_t *) &texture->depth, STBI_rgb_alpha);

    texture->depth = STBI_rgb_alpha; // TODO: Consider the normal and metallic roughness channels
    texture->mips = 1;
    texture->size = texture->width * texture->height * texture->depth;

    debug("\t\tDepth: %u", texture->depth);
    debug("\t\tLoaded Levels: %u", texture->mips);
    debug("\t\tRaw Size: %lu", texture->size);
    debug("\t\tRaw texture loaded");
}

void generateRawMipmaps(PRawTexture texture) {
    size_t size = 4 * texture->size / 3; // NOTICE: Sum of mip size series
    void *result = realloc(texture->data, size);

    if(result == nullptr) {
        debug("\t\tAllocation for raw mipmap generation failed");
        free(texture->data);
        assert(result != nullptr);
    }

    texture->data = result;
    debug("\t\tReallocated Size: %lu", size);

    texture->mips = (uint32_t) floor(log2(umax(texture->width, texture->height))) + 1;

    int32_t sourceWidth  = (int32_t) texture->width;
    int32_t sourceHeight = (int32_t) texture->height;
    size_t  sourceOffset = 0;
    size_t  sourceSize   = texture->size;

    for(uint32_t level = 1; level < texture->mips; level++) {
        int32_t destinationWidth  = imax(sourceWidth  / 2, 1);
        int32_t destinationHeight = imax(sourceHeight / 2, 1);
        size_t  destinationOffset = sourceOffset + sourceSize;
        size_t  destinationSize   = destinationWidth * destinationHeight * texture->depth;

        if(texture->isColor) {
            stbir_resize_uint8_srgb(  texture->data + sourceOffset, sourceWidth, sourceHeight, 0, texture->data + destinationOffset, destinationWidth, destinationHeight, 0, STBIR_RGBA);
        } else {
            stbir_resize_uint8_linear(texture->data + sourceOffset, sourceWidth, sourceHeight, 0, texture->data + destinationOffset, destinationWidth, destinationHeight, 0, STBIR_RGBA);
        }

        texture->size += destinationSize;

        sourceWidth  = destinationWidth;
        sourceHeight = destinationHeight;
        sourceOffset = destinationOffset;
        sourceSize   = destinationSize;
    }

    debug("\t\tTotal Used Size:  %lu", texture->size);
    debug("\t\tFinal Level Count: %u", texture->mips);
    debug("\t\tRaw mipmaps generated");
}

PCompressedTexture convertRawTexture(PRawTexture rawTexture) {
    PCompressedTexture convertedTexture = malloc(sizeof(CompressedTexture));

    ktxTextureCreateInfo compressedTextureCreateInfo = {
        .glInternalformat = 0, // Ignored
        .vkFormat = rawTexture->isColor ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        .pDfd = nullptr, // Ignored
        .baseWidth = rawTexture->width,
        .baseHeight = rawTexture->height,
        .baseDepth = 1,
        .numDimensions = 2,
        .numLevels = (uint32_t) floor(log2(umax(rawTexture->width, rawTexture->height))) + 1,
        .numLayers = 1,
        .numFaces = 1,
        .isArray = KTX_FALSE,
        .generateMipmaps = KTX_FALSE
    };

    ktx_error_code_e result = ktxTexture2_Create(&compressedTextureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &convertedTexture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tCreating texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    // TODO: Make sure that this is not inferred from ktxTextureCreateInfo.vkFormat of ktxTexture2_Create
    if(rawTexture->isColor) {
        result = ktxTexture2_SetTransferFunction(convertedTexture->handle, KHR_DF_TRANSFER_SRGB);
    } else {
        result = ktxTexture2_SetTransferFunction(convertedTexture->handle, KHR_DF_TRANSFER_LINEAR);
    }

    if(result != KTX_SUCCESS) {
        debug("\t\tSetting transfer function failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    convertedTexture->isColor = rawTexture->isColor;
    convertedTexture->compatibilityHandle = (ktxTexture *) convertedTexture->handle;

    size_t   offset = 0;
    uint32_t width  = rawTexture->width;
    uint32_t height = rawTexture->height;
    size_t   size   = rawTexture->width * rawTexture->height * rawTexture->depth;

    for(uint32_t level = 0; level < rawTexture->mips; level++) {
        ktxTexture_SetImageFromMemory(convertedTexture->compatibilityHandle, level, 0, 0, rawTexture->data + offset, size);

        offset += size;
        width   = umax(width  / 2, 1);
        height  = umax(height / 2, 1);
        size    = width * height * rawTexture->depth;
    }

    stbi_image_free(rawTexture->data);
    free(rawTexture);

    debug("\t\tConverted Size: %lu", ktxTexture_GetDataSize(convertedTexture->compatibilityHandle));
    debug("\t\tRaw texture converted");

    return convertedTexture;
}

void generateConvertedMipmaps(PCompressedTexture texture) {
    int32_t sourceWidth  = (int32_t) texture->handle->baseWidth;
    int32_t sourceHeight = (int32_t) texture->handle->baseHeight;

    size_t sourceOffset = 0;
    ktx_error_code_e result = ktxTexture2_GetImageOffset(texture->handle, 0, 0, 0, &sourceOffset);

    if(result != KTX_SUCCESS) {
        debug("\t\tGetting base image offset failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    for(uint32_t level = 1; level < texture->handle->numLevels; level++) {
        int32_t destinationWidth  = imax(sourceWidth  / 2, 1);
        int32_t destinationHeight = imax(sourceHeight / 2, 1);

        size_t destinationOffset = 0;
        result = ktxTexture2_GetImageOffset(texture->handle, level, 0, 0, &destinationOffset);

        if(result != KTX_SUCCESS) {
            debug("\t\tGetting level %u image offset failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        if(texture->isColor) {
            stbir_resize_uint8_srgb(  texture->handle->pData + sourceOffset, sourceWidth, sourceHeight, 0, texture->handle->pData + destinationOffset, destinationWidth, destinationHeight, 0, STBIR_RGBA);
        } else {
            stbir_resize_uint8_linear(texture->handle->pData + sourceOffset, sourceWidth, sourceHeight, 0, texture->handle->pData + destinationOffset, destinationWidth, destinationHeight, 0, STBIR_RGBA);
        }

        sourceWidth  = destinationWidth;
        sourceHeight = destinationHeight;
        sourceOffset = destinationOffset;
    }

    debug("\t\tFinal Level Count: %u", texture->handle->numLevels);
    debug("\t\tConverted mipmaps generated");
}

void compressConvertedTexture(PCompressedTexture texture) {
    ktxBasisParams compressionParameters = {
        .structSize = sizeof(ktxBasisParams),
        .uastc = KTX_TRUE, // TODO: Dive further into that compression optimization rabbit hole
        .threadCount = threadCount,
        .compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL,
        //.normalMap = !isColor
        .normalMap = false // TODO: Research that topic to see how that can be used for metallic roughness and normal maps
    }; // NOTICE: Many more params exist here that are zero initialized

    ktx_error_code_e result = ktxTexture2_CompressBasisEx(texture->handle, &compressionParameters);

    if(result != KTX_SUCCESS) {
        debug("\t\tCompressing texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    debug("\t\tCompressed Size: %lu", ktxTexture_GetDataSize(texture->compatibilityHandle));
    debug("\t\tConverted texture compressed");
}

PCompressedTexture initializeCompressedTexture(const char *subdirectory, const char *filename, bool isColor) {
    PCompressedTexture texture = malloc(sizeof(CompressedTexture));

    char path[PATH_MAX];
    makeFullPath(subdirectory, filename, path);
    debug("\tImage Path: %s", path);

    ktx_error_code_e result = ktxTexture2_CreateFromNamedFile(path, 0, &texture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    texture->compatibilityHandle = (ktxTexture*) texture->handle;
    texture->isColor = isColor;

    debug("\t\tWidth:  %u", texture->handle->baseWidth);
    debug("\t\tHeight: %u", texture->handle->baseHeight);
    debug("\t\tDepth:  %u", texture->handle->baseDepth);
    debug("\t\tLevels: %u", texture->handle->numLevels);
    debug("\t\tAllocated Size: %lu", ktxTexture_GetDataSize(texture->compatibilityHandle));
    debug("\t\tCompressed texture initialized");

    return texture;
}

void loadCompressedTexture(PCompressedTexture texture) {
    size_t size = ktxTexture_GetDataSize(texture->compatibilityHandle);
    texture->handle->pData = malloc(size);

    ktx_error_code_e result = ktxTexture2_LoadImageData(texture->handle, nullptr, size);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture data failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    debug("\t\tLoaded Size: %lu", size);
    debug("\t\tCompressed texture loaded");
}

void transcodeCompressedTexture(PCompressedTexture texture) {
    if(ktxTexture2_NeedsTranscoding(texture->handle)) {
        ktx_error_code_e result = ktxTexture2_TranscodeBasis(texture->handle, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        debug("\t\tTranscoded Size: %lu", ktxTexture_GetDataSize(texture->compatibilityHandle));
        debug("\t\tCompressed texture transcoded");
    }
}

PImage createTextureImage(PCompressedTexture texture) {
    PImage image = createImage(texture->handle->baseWidth, texture->handle->baseHeight, texture->handle->numLevels, VK_SAMPLE_COUNT_1_BIT, texture->isColor ? VK_FORMAT_BC7_SRGB_BLOCK : VK_FORMAT_BC7_UNORM_BLOCK, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_TILING_OPTIMAL);
    bindImageMemory(image, &deviceMemory);
    transitionImageLayout(image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    debug("\t\tAligned Size:  %lu", image->memoryRequirements.size);
    debug("\t\tMemory Offset: %lu", image->memoryOffset);
    debug("\t\tTexture image created");

    return image;
}

void loadTextureImage(PImage image, PCompressedTexture texture) {
    uint8_t *data = ktxTexture_GetData(texture->compatibilityHandle);

    for(ktx_uint32_t level = 0; level < texture->handle->numLevels; level++) {
        size_t offset = 0;
        ktx_error_code_e result = ktxTexture2_GetImageOffset(texture->handle, level, 0, 0, &offset);

        if(result != KTX_SUCCESS) {
            debug("\t\tGetting mip level %u data failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        size_t size = ktxTexture_GetImageSize(texture->compatibilityHandle, level);

        if(size <= sharedBuffer.size) {
            memcpy(mappedSharedMemory, data + offset, size);
            copyBufferToImage(&sharedBuffer, 0, image, level);
        } else if (size <= deviceBuffer.size) {
            stagingBufferCopy(data, offset, 0, size);
            copyBufferToImage(&deviceBuffer, 0, image, level);
        } else {
            debug("\t\tCan't copy image data, increase shared or device local buffer size!");
            assert(size <= sharedBuffer.size || size <= deviceBuffer.size);
        }
    }

    ktxTexture2_Destroy(texture->handle);
    free(texture);

    transitionImageLayout(image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    createImageView(image);

    debug("\t\tTexture image loaded into memory");
}
