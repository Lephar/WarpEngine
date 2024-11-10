#include "config.h"

#include "helper.h"
#include "queue.h"
#include "shader.h"
#include "manager.h"

char rootPath[PATH_MAX];
char executableName[PATH_MAX];

extern VkExtent2D extent;

extern Queue graphicsQueue;
extern Queue  computeQueue;
extern Queue transferQueue;

Queue *queueReferences[] = {
    &graphicsQueue,
    & computeQueue,
    &transferQueue
};

uint32_t queueCount;

extern Shader vertex;
extern Shader fragment;

Shader *shaderReferences[] = {
    &vertex,
    &fragment
};

uint32_t shaderCount;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    char *separator = strrchr(argv[0], '/');

    if(separator == NULL) {
        assert(getcwd(rootPath, PATH_MAX) != NULL);
        assert(snprintf(executableName, PATH_MAX, "%s", argv[0]) < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game
        assert(snprintf(rootPath, PATH_MAX, "%s", argv[0]) < PATH_MAX);
        assert(snprintf(executableName, PATH_MAX, "%s", separator) < PATH_MAX);
    }

    debug("Path:   %s", rootPath);
    debug("Name:   %s", executableName);

    char config[PATH_MAX];

    if(argc < 2) {
        assert(snprintf(config, PATH_MAX, "%s/config.txt", rootPath) < PATH_MAX);
    } else {
        assert(snprintf(config, PATH_MAX, "%s/%s", rootPath, argv[1]) < PATH_MAX);
    }

    debug("Config: %s", config);

    FILE *file = fopen(config, "r");
    assert(file != NULL);

     queueCount = sizeof( queueReferences) / sizeof(Queue  *);
    shaderCount = sizeof(shaderReferences) / sizeof(Shader *);

    debug("Queue  count: %d",  queueCount);
    debug("Shader count: %d", shaderCount);

    char discard[PATH_MAX];

    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Window:", PATH_MAX) == 0 || strncmp(discard, "Fullscreen:", PATH_MAX) == 0);

    fscanf(file, "%u%u", &extent.width, &extent.height);
    debug("Width:  %u", extent.width );
    debug("Height: %u", extent.height);

    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Shaders:", PATH_MAX) == 0);

    uint32_t configShaderCount;
    fscanf(file, "%u", &configShaderCount);
    assert(configShaderCount == shaderCount);

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        Shader *shader = shaderReferences[shaderIndex];

        char kind[UINT8_MAX];
        char intermediate[UINT8_MAX];

        fscanf(file, "%s%s%s", shader->name, kind, intermediate);

        debug("Shader: %s", shader->name);
        debug("\tKind:          %s", kind);
        debug("\tIntermediate:  %s", intermediate);

        if(strncmp(kind, "compute", UINT8_MAX) == 0) {
            shader->kind = shaderc_compute_shader;
        } else if(strncmp(kind, "vertex", UINT8_MAX) == 0) {
            shader->kind = shaderc_vertex_shader;
        } else if(strncmp(kind, "fragment", UINT8_MAX) == 0) {
            shader->kind = shaderc_fragment_shader;
        } //TODO: Add other shader types

        if(strncmp(intermediate, "intermediate", UINT8_MAX) == 0 || strncmp(intermediate, "spirv", UINT8_MAX) == 0) {
            shader->intermediate = VK_TRUE;
        } else {
            shader->intermediate = VK_FALSE;
        }
    }

    // Discard rest of the file for now
    fclose(file);
}
