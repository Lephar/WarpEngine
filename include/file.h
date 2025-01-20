#pragma once

#include "pch.h"

struct file {
    char fullpath[PATH_MAX];
    size_t size;
    char *data;
} typedef File;

struct shaderFile {
    File file;
    VkShaderStageFlags stage;
    VkBool32 intermediate;
} typedef ShaderFile;

struct modelFile {
    File file;
    uint32_t binary;
} typedef ModelFile;

// WARN: Free the output data!
File readFile(const char *relativePath, uint32_t binary);