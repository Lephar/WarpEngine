#include "zero.h"

struct shader {
    size_t size;
    char *data;
    VkShaderModule module;
} typedef Shader;

void loadShaders();
void createModules();
void createDescriptors();
void destroyModules();
void destroyDescriptors();
