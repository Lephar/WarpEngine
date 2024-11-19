#pragma once

#include "zero.h"

struct shader {
    char name[PATH_MAX];
    VkShaderStageFlags stage;
    VkBool32 intermediate;
    size_t size;
    char *data;
    VkShaderEXT module;
} typedef Shader;

void createDescriptors();
void createModules();
void destroyDescriptors();
void destroyModules();
