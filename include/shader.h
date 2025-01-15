#pragma once

#include "pch.h"

struct shader {
    size_t size;
    char *data;
    VkShaderEXT module;
} typedef Shader;

void createDescriptors();
void createModules();
void destroyDescriptors();
void destroyModules();
