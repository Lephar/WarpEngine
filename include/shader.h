#pragma once

#include "pch.h"

typedef enum fileType FileType;

typedef struct data Data;

struct shaderCode {
    FileType *type;
    shaderc_shader_kind stage;
    Data *data;
} typedef ShaderCode;

struct shaderModule {
    VkShaderStageFlagBits stage;
    VkShaderStageFlagBits nextStage;
    VkShaderEXT module;
} typedef ShaderModule;

extern ShaderModule   skyboxShaderModule;
extern ShaderModule   vertexShaderModule;
extern ShaderModule fragmentShaderModule;

void createModules();
void destroyModules();
