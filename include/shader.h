#pragma once

#include "pch.h"

typedef struct data Data;

struct shaderCode {
    shaderc_shader_kind stage;
    Data *data;
} typedef ShaderCode;

struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderStageFlagBits nextStage;
    VkShaderEXT module;
} typedef ShaderModule;

extern ShaderModule vertexShaderModule;
extern ShaderModule fragmentShaderModule;

void createModules();
void bindShaders(VkCommandBuffer commandBuffer, ShaderModule *vertexShader, ShaderModule *fragmentShader);
void destroyModules();
