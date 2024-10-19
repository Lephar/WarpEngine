#include "zero.h"

struct shader {
    const char *path;
    shaderc_shader_kind kind;
    VkShaderModule module;
} typedef Shader;
