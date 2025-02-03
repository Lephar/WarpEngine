#pragma once

#include "pch.h"

#include "file.h"

struct shaderCode {
    FileType type;
    shaderc_shader_kind stage;
    Data data;
} typedef ShaderCode;

struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderEXT module;
} typedef ShaderModule;

void createDescriptors();
void createModules();
void destroyDescriptors();
void destroyModules();
