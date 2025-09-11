#include "texture.h"

#include "physicalDevice.h"

#include "logger.h"
#include "config.h"
#include "file.h"
#include "numerics.h"

PTextureInfo makeTextureInfo(const char *subdirectory, const char *filename, bool isColorTexture) {
    PTextureInfo info = malloc(sizeof(TextureInfo));

    makeFullPath(subdirectory, filename, info->path);
    debug("\tImage Path: %s", info->path);

    info->isColorTexture = isColorTexture;
    info->depth = STBI_rgb_alpha; // TODO: Consider the normal and metallic roughness channels
    info->size = 0;

    return info;
}

PRawTexture initializeRawTexture(const char *subdirectory, const char *filename, bool isColorTexture) {
    PRawTexture texture = malloc(sizeof(RawTexture));

    texture->info = makeTextureInfo(subdirectory, filename, isColorTexture);
    texture->data = nullptr;

    int32_t result = stbi_info(texture->info->path, (int32_t *) &texture->info->width, (int32_t*) &texture->info->height, nullptr);

    // TODO: Check error result and string

    assert((uint32_t) texture->info->width <= physicalDeviceProperties.limits.maxImageDimension2D && (uint32_t) texture->info->height <= physicalDeviceProperties.limits.maxImageDimension2D);

    texture->info->mips = (uint32_t) floor(log2(umax(texture->info->width, texture->info->height))) + 1;

    debug("\t\tWidth:  %u", texture->info->width);
    debug("\t\tHeight: %u", texture->info->height);
    debug("\t\tDepth:  %u", texture->info->depth);
    debug("\t\tMips:   %u", texture->info->mips);

    return texture;
}

void loadRawTexture(PRawTexture texture) {
    // NOTICE: Allocates data double the necessary size because of our STBI_MALLOC override in implementation.c
    texture->data = stbi_load(texture->info->path, nullptr, nullptr, nullptr, (int32_t) texture->info->depth);
    texture->info->size = texture->info->width * texture->info->height * texture->info->depth;
}

void generateRawMipmaps(PRawTexture texture) {
    uint32_t srcWidth  = texture->info->width;
    uint32_t srcHeight = texture->info->height;
    size_t   srcOffset = 0;
    size_t   srcSize   = texture->info->size;

    for(uint32_t level = 1; level < texture->info->mips; level++) {
        uint32_t dstWidth  = umax(srcWidth  / 2, 1);
        uint32_t dstHeight = umax(srcHeight / 2, 1);
        size_t   dstOffset = srcOffset + srcSize;
        size_t   dstSize   = dstWidth * dstHeight * texture->info->depth;

        if(texture->info->isColorTexture) {
            stbir_resize_uint8_srgb(texture->data + srcOffset, (int32_t) srcWidth, (int32_t) srcHeight, 0, texture->data + dstOffset, (int32_t) dstWidth, (int32_t) dstHeight, 0, STBIR_RGBA);
        } else {
            stbir_resize_uint8_linear(texture->data + srcOffset, (int32_t) srcWidth, (int32_t) srcHeight, 0, texture->data + dstOffset, (int32_t) dstWidth, (int32_t) dstHeight, 0, STBIR_RGBA);
        }

        texture->info->size += dstSize;

        srcWidth  = dstWidth;
        srcHeight = dstHeight;
        srcOffset = dstOffset;
        srcSize   = dstSize;
    }
}

PCompressedTexture compressRawTexture(PRawTexture rawTexture) {
    PCompressedTexture compressedTexture = malloc(sizeof(CompressedTexture));
    compressedTexture->info = rawTexture->info;

    ktxTextureCreateInfo compressedTextureCreateInfo = {
        .glInternalformat = 0, // Ignored
        .vkFormat = compressedTexture->info->isColorTexture ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        .pDfd = nullptr, // Ignored
        .baseWidth = compressedTexture->info->width,
        .baseHeight = compressedTexture->info->height,
        .baseDepth = 1,
        .numDimensions = 2,
        .numLevels = compressedTexture->info->mips,
        .numLayers = 1,
        .numFaces = 1,
        .isArray = KTX_FALSE,
        .generateMipmaps = KTX_FALSE
    };

    // TODO: Can storage be set directly to the data?
    ktx_error_code_e result = ktxTexture2_Create(&compressedTextureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &compressedTexture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tCreating texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    ktxTexture *compatibilityHandle = (ktxTexture*) compressedTexture->handle;

    debug("\t\tRaw Size:   %lu", rawTexture->info->size);

    compressedTexture->info->size = ktxTexture_GetDataSize(compatibilityHandle);
    debug("\t\tFinal Size: %lu", compressedTexture->info->size);

    for(uint32_t level = 0; level < compressedTexture->info->mips; level++) {
        size_t offset;
        result = ktxTexture2_GetImageOffset(compressedTexture->handle, level, 0, 0, &offset);

        if(result != KTX_SUCCESS) {
            debug("\t\tGetting mip level %u data failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        size_t size = ktxTexture_GetImageSize(compatibilityHandle, level);

        // TODO: Make sure stbi and ktx offsets match
        result = ktxTexture_SetImageFromMemory(compatibilityHandle, level, 0, 0, rawTexture->data + offset, size);

        if(result != KTX_SUCCESS) {
            debug("\t\tSetting mip level %u from memory failed with message: %s", level, ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }
    }

    stbi_image_free(rawTexture->data);
    free(rawTexture);

    // TODO: IS THIS NECESSARY?
    /*ktxBasisParams compressionParameters = {
        .structSize = sizeof(ktxBasisParams),
        .uastc = KTX_TRUE, // TODO: Dive further into that compression optimization rabbit hole
        .threadCount = threadCount,
        .compressionLevel = KTX_ETC1S_DEFAULT_COMPRESSION_LEVEL,
        .normalMap = false // TODO: Research that topic to see how that can be used for metallic roughness and normal maps
    }; // NOTICE: Many more params exist here that are zero initialized here

    result = ktxTexture2_CompressBasisEx(compressedTexture->handle, &compressionParameters);

    if(result != KTX_SUCCESS) {
        debug("\t\tCompressing texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }*/

    if(ktxTexture2_NeedsTranscoding(compressedTexture->handle)) {
        result = ktxTexture2_TranscodeBasis(compressedTexture->handle, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        debug("\t\tTranscoded Size: %lu", ktxTexture_GetDataSize(compatibilityHandle));
    }

    return compressedTexture;
}

PCompressedTexture initializeCompressedTexture(const char *subdirectory, const char *filename, bool isColorTexture) {
    PCompressedTexture texture = malloc(sizeof(CompressedTexture));

    texture->info = makeTextureInfo(subdirectory, filename, isColorTexture);
    texture->data = nullptr;

    ktx_error_code_e result = ktxTexture2_CreateFromNamedFile(texture->info->path, 0, &texture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    texture->info->width  = texture->handle->baseWidth;
    texture->info->height = texture->handle->baseHeight;
    texture->info->depth  = texture->handle->baseDepth;
    texture->info->mips   = texture->handle->numLevels;

    debug("\t\tWidth:  %u", texture->info->width);
    debug("\t\tHeight: %u", texture->info->height);
    debug("\t\tDepth:  %u", texture->info->depth);
    debug("\t\tMips:   %u", texture->info->mips);

    return texture;
}

void loadCompressedTexture(PCompressedTexture texture) {
    ktxTexture2_Destroy(texture->handle);

    ktx_error_code_e result = ktxTexture2_CreateFromNamedFile(texture->info->path, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    ktxTexture *compatibilityHandle = (ktxTexture*) texture->handle;

    texture->info->size = ktxTexture_GetDataSize(compatibilityHandle);
    debug("\t\tCompressed Size: %lu", texture->info->size);

    if(ktxTexture2_NeedsTranscoding(texture->handle)) {
        result = ktxTexture2_TranscodeBasis(texture->handle, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        texture->info->size = ktxTexture_GetDataSize(compatibilityHandle);
        debug("\t\tTranscoded Size: %lu", texture->info->size);
    }

    texture->data = ktxTexture_GetData(compatibilityHandle);
}
