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
    VkShaderStageFlagBits nextStage;
    VkShaderEXT module;
} typedef ShaderModule;

void createModules();
void destroyModules();
