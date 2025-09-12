#pragma once

#include "pch.h"

typedef struct textureInfo {
    char path[PATH_MAX];

    uint32_t isColor;

    size_t size;
} TextureInfo, *PTextureInfo;

typedef struct rawTexture {
    PTextureInfo info;

    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mips;

    stbi_uc *data;
} RawTexture, *PRawTexture;

typedef struct compressedTexture {
    PTextureInfo info;

    uint32_t isCompressed;
    uint32_t isTranscoded;

    ktxTexture2 *handle;
    ktxTexture  *compatibilityHandle;
} CompressedTexture, *PCompressedTexture;