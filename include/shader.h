#pragma once

#include "pch.h"

typedef struct data Data;

typedef struct shaderCode {
    shaderc_shader_kind stage;
    size_t size;
    char *data;
} ShaderCode, *PShaderCode;

typedef struct shaderIntermediate {
    shaderc_shader_kind stage;
    size_t size;
    uint32_t *data;
} ShaderIntermediate, *PShaderIntermediate;

typedef struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderStageFlagBits nextStage;
    VkShaderEXT module;
} ShaderModule, *PShaderModule;

extern ShaderModule *vertexShaderModule;
extern ShaderModule *fragmentShaderModule;

void createModules();
void bindShaders(VkCommandBuffer commandBuffer, ShaderModule *vertexShader, ShaderModule *fragmentShader);
void destroyModules();
