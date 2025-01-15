#pragma once

#include "pch.h"

struct file {
    char folder[PATH_MAX];
    char name[PATH_MAX];
    char extension[PATH_MAX];
    char fullpath[PATH_MAX];
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

void configure(int argc, char *argv[]);
