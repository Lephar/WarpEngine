#include "zero.h"

struct shader {
    const char *name;
    shaderc_shader_kind kind;
    char file[PATH_MAX];
    size_t size;
    char *code; // WARN: Not used for .spv
    uint32_t *intermediate;
    VkShaderModule module;
} typedef Shader;

void createModules();
void createDescriptors();
void destroyModules();
void destroyDescriptors();