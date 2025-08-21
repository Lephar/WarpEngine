#pragma once

#include "pch.h"

typedef struct data Data;

struct shaderCode {
    shaderc_shader_kind stage;
    size_t size;
    char *data;
} typedef ShaderCode;

struct shaderIntermediate {
    shaderc_shader_kind stage;
    size_t size;
    uint32_t *data;
} typedef ShaderIntermediate;

struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderStageFlagBits nextStage;
    VkShaderEXT module;
} typedef ShaderModule;

extern ShaderModule *vertexShaderModule;
extern ShaderModule *fragmentShaderModule;

void createModules();
void bindShaders(VkCommandBuffer commandBuffer, ShaderModule *vertexShader, ShaderModule *fragmentShader);
void destroyModules();
