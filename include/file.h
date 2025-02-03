#pragma once

#include "pch.h"

enum fileType {
    FILE_TYPE_TEXT,
    FILE_TYPE_BINARY
} typedef FileType;

struct file {
    FileType type;
    size_t size;
    char *data;
} typedef File;

struct shaderIntermediate {
    shaderc_shader_kind stage;
    size_t size;
    char *data;
} typedef ShaderIntermediate;

struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderEXT module;
} typedef ShaderModule;

struct sceneFile {
    uint32_t binary;
} typedef SceneFile;

// WARN: Free the output data!
File readFile(const char *path, FileType type);
