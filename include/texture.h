#pragma once

#include "pch.h"

typedef struct image *PImage;

typedef struct rawTexture {
    char path[PATH_MAX];

    uint32_t isColor;

    uint32_t width;
    uint32_t height;
    uint32_t depth;

    uint32_t mips;
    size_t   size;

    stbi_uc *data;
} RawTexture, *PRawTexture;

typedef struct compressedTexture {
    uint32_t isColor;

    ktxTexture2 *handle;
    ktxTexture  *compatibilityHandle;
} CompressedTexture, *PCompressedTexture;

PRawTexture initializeRawTexture(const char *subdirectory, const char *filename, bool isColor);
void loadRawTexture(PRawTexture texture);
void generateRawMipmaps(PRawTexture texture);
PCompressedTexture convertRawTexture(PRawTexture rawTexture);
PCompressedTexture convertRawBaseTexture(PRawTexture rawTexture);
void generateConvertedMipmaps(PCompressedTexture texture);
void compressConvertedTexture(PCompressedTexture texture);
PCompressedTexture initializeCompressedTexture(const char *subdirectory, const char *filename, bool isColor);
void loadCompressedTexture(PCompressedTexture texture);
void transcodeCompressedTexture(PCompressedTexture texture);
PImage createTextureImage(PCompressedTexture texture);
void loadTextureImage(PImage image, PCompressedTexture texture);
