#include "texture.h"

#include "buffer.h"
#include "physicalDevice.h"
#include "memory.h"

#include "logger.h"
#include "file.h"
#include "image.h"
#include "numerics.h"

PTextureInfo makeTextureInfo(const char *subdirectory, const char *filename, bool isColor) {
    PTextureInfo info = malloc(sizeof(TextureInfo));

    makeFullPath(subdirectory, filename, info->path);
    debug("\tImage Path: %s", info->path);

    info->isColor = isColor;
    info->size    = 0;

    debug("\t\tInitial Size: %lu", info->size);
    debug("\t\t%s texture info initialized", info->isColor ? "Color" : "Non-color");

    return info;
}

PRawTexture initializeRawTexture(const char *subdirectory, const char *filename, bool isColor) {
    PRawTexture texture = malloc(sizeof(RawTexture));

    texture->info  = makeTextureInfo(subdirectory, filename, isColor);

    int32_t result = stbi_info(texture->info->path, (int32_t *) &texture->width, (int32_t *) &texture->height, (int32_t *) &texture->depth);

    if(result != 0) {
        debug("\t\tImage meta data loading failed with message: %s", stbi_failure_reason());
        assert(result == 0);
    }

    assert((uint32_t) texture->width <= physicalDeviceProperties.limits.maxImageDimension2D && (uint32_t) texture->height <= physicalDeviceProperties.limits.maxImageDimension2D);

    texture->mips = 0;
    texture->data = nullptr;

    debug("\t\tWidth:  %u", texture->width);
    debug("\t\tHeight: %u", texture->height);
    debug("\t\tDepth:  %u", texture->depth);

    return texture;
}

void loadRawTexture(PRawTexture texture) {
    // NOTICE: Allocates double the necessary size for data because of our STBI_MALLOC override in implementation.c
    texture->data = stbi_load(texture->info->path, nullptr, nullptr, nullptr, STBI_rgb_alpha);

    texture->depth = STBI_rgb_alpha; // TODO: Consider the normal and metallic roughness channels
    texture->mips = 1;
    texture->info->size = texture->width * texture->height * texture->depth;

    debug("\t\tMips:   %u", texture->mips);
    debug("\t\tSize:   %u", texture->info->size);
}

void generateRawMipmaps(PRawTexture texture) {
    texture->mips = (uint32_t) floor(log2(umax(texture->width, texture->height))) + 1;

    uint32_t sourceWidth  = texture->width;
    uint32_t sourceHeight = texture->height;
    size_t   sourceOffset = 0;
    size_t   sourceSize   = texture->info->size;

    for(uint32_t level = 1; level < texture->mips; level++) {
        uint32_t destinationWidth  = umax(sourceWidth  / 2, 1);
        uint32_t destinationHeight = umax(sourceHeight / 2, 1);
        size_t   destinationOffset = sourceOffset + sourceSize;
        size_t   destinationSize   = destinationWidth * destinationHeight * texture->depth;

        if(texture->info->isColor) {
            stbir_resize_uint8_srgb(  texture->data + sourceOffset, (int32_t) sourceWidth, (int32_t) sourceHeight, 0, texture->data + destinationOffset, (int32_t) destinationWidth, (int32_t) destinationHeight, 0, STBIR_RGBA);
        } else {
            stbir_resize_uint8_linear(texture->data + sourceOffset, (int32_t) sourceWidth, (int32_t) sourceHeight, 0, texture->data + destinationOffset, (int32_t) destinationWidth, (int32_t) destinationHeight, 0, STBIR_RGBA);
        }

        texture->info->size += destinationSize;

        sourceWidth  = destinationWidth;
        sourceHeight = destinationHeight;
        sourceOffset = destinationOffset;
        sourceSize   = destinationSize;
    }
}

PCompressedTexture compressRawTexture(PRawTexture rawTexture) {
    PCompressedTexture compressedTexture = malloc(sizeof(CompressedTexture));

    ktxTextureCreateInfo compressedTextureCreateInfo = {
        .glInternalformat = 0, // Ignored
        .vkFormat = rawTexture->info->isColor ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
        .pDfd = nullptr, // Ignored
        .baseWidth = rawTexture->width,
        .baseHeight = rawTexture->height,
        .baseDepth = 1,
        .numDimensions = 2,
        .numLevels = rawTexture->mips,
        .numLayers = 1,
        .numFaces = 1,
        .isArray = KTX_FALSE,
        .generateMipmaps = KTX_FALSE
    };

    ktx_error_code_e result = ktxTexture2_Create(&compressedTextureCreateInfo, KTX_TEXTURE_CREATE_NO_STORAGE, &compressedTexture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tCreating texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    compressedTexture->handle->pData = rawTexture->data;
    compressedTexture->info = rawTexture->info;

    free(rawTexture);

    return compressedTexture;
}

PCompressedTexture initializeCompressedTexture(const char *subdirectory, const char *filename, bool isColor) {
    PCompressedTexture texture = malloc(sizeof(CompressedTexture));

    texture->info = makeTextureInfo(subdirectory, filename, isColor);

    ktx_error_code_e result = ktxTexture2_CreateFromNamedFile(texture->info->path, 0, &texture->handle);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    debug("\t\tWidth:  %u", texture->handle->baseWidth);
    debug("\t\tHeight: %u", texture->handle->baseHeight);
    debug("\t\tDepth:  %u", texture->handle->baseDepth);
    debug("\t\tMips:   %u", texture->handle->numLevels);

    texture->compatibilityHandle = (ktxTexture*) texture->handle;

    return texture;
}

void loadCompressedTexture(PCompressedTexture texture) {
    //texture->info->size = ktxTexture_GetDataSizeUncompressed(texture->compatibilityHandle);
    texture->info->size = ktxTexture_GetDataSize(texture->compatibilityHandle);
    texture->handle->pData = malloc(texture->info->size);

    ktx_error_code_e result = ktxTexture2_LoadImageData(texture->handle, nullptr, texture->info->size);

    if(result != KTX_SUCCESS) {
        debug("\t\tLoading texture data failed with message: %s", ktxErrorString(result));
        assert(result == KTX_SUCCESS);
    }

    // TODO: Done?
}

void transcodeCompressedTexture(PCompressedTexture texture) {
    if(ktxTexture2_NeedsTranscoding(texture->handle)) {
        ktx_error_code_e result = ktxTexture2_TranscodeBasis(texture->handle, KTX_TTF_BC7_RGBA, 0);

        if(result != KTX_SUCCESS) {
            debug("\t\tTranscoding texture failed with message: %s", ktxErrorString(result));
            assert(result == KTX_SUCCESS);
        }

        texture->info->size = ktxTexture_GetDataSize(texture->compatibilityHandle);
        debug("\t\tTranscoded Size: %lu", texture->info->size);
    }
}
