#pragma once

#include "pch.h"

typedef struct image Image;

typedef struct textureInfo {
    char path[PATH_MAX];

    uint32_t isColorTexture;

    uint32_t width;
    uint32_t height;
    uint32_t depth;

    uint32_t mips;
    size_t   size;
} TextureInfo, *PTextureInfo;

typedef struct rawTexture {
    TextureInfo *info;

    stbi_uc *data;
} RawTexture, *PRawTexture;

typedef struct compressedTexture {
    TextureInfo *info;

    ktxTexture2 *handle;
    ktx_uint8_t *data;
} CompressedTexture, *PCompressedTexture;
