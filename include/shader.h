#include "zero.h"

struct shader {
    const char *name;
    shaderc_shader_kind kind;
    char filename[PATH_MAX];
    size_t codeSize;
    char *code;
    size_t intermediateSize;
    uint32_t *intermediate;
    VkShaderModule module;
} typedef Shader;

void createModules();
void createDescriptors();
void destroyModules();
void destroyDescriptors();
