#include "config.h"

#include "helper.h"
#include "queue.h"
#include "shader.h"
#include "content.h"

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

extern Shader vertexShader;
extern Shader fragmentShader;

Shader *shaderReferences[] = {
    &vertexShader,
    &fragmentShader
};

uint32_t shaderCount;

void configure(int argc, char *argv[]) {
    debug("argc: %d", argc);
    for(int32_t argn = 0; argn < argc; argn++) {
        debug("\targv[%d]: %s", argn, argv[argn]);
    }

    char *separator = strrchr(argv[0], '/');

    char *result = NULL;
    int32_t length = 0;

    if(separator == NULL) {
        result = getcwd(rootPath, PATH_MAX);
        assert(result != NULL);

        length = snprintf(executableName, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);
    } else {
        *separator++ = '\0'; // Let's play a game

        length = snprintf(rootPath, PATH_MAX, "%s", argv[0]);
        assert(length < PATH_MAX);

        length = snprintf(executableName, PATH_MAX, "%s", separator);
        assert(length < PATH_MAX);
    }

    debug("Path:   %s", rootPath);
    debug("Name:   %s", executableName);

    char config[PATH_MAX];

    if(argc < 2) {
        length = snprintf(config, PATH_MAX, "%s/config.txt", rootPath);
        assert(length < PATH_MAX);
    } else {
        length = snprintf(config, PATH_MAX, "%s/%s", rootPath, argv[1]);
        assert(length < PATH_MAX);
    }

    debug("Config: %s", config);

    FILE *file = fopen(config, "r");
    assert(file != NULL);

    char discard[PATH_MAX];

    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Window:", PATH_MAX) == 0 || strncmp(discard, "Fullscreen:", PATH_MAX) == 0);

    fscanf(file, "%u%u", &extent.width, &extent.height);
    debug("Width:  %u", extent.width );
    debug("Height: %u", extent.height);

     queueCount = sizeof( queueReferences) / sizeof(Queue  *);
    shaderCount = sizeof(shaderReferences) / sizeof(Shader *);

    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Shaders:", PATH_MAX) == 0);

    uint32_t configShaderCount;
    fscanf(file, "%u", &configShaderCount);
    debug("Shader count: %d", configShaderCount);
    assert(configShaderCount == shaderCount);

    for(uint32_t shaderIndex = 0; shaderIndex < shaderCount; shaderIndex++) {
        Shader *shader = shaderReferences[shaderIndex];

        char kind[UINT8_MAX];
        char type[UINT8_MAX];

        fscanf(file, "%s%s%s", shader->name, kind, type);

        debug("Shader: %s", shader->name);
        debug("\tKind: %s", kind);
        debug("\tType: %s", type);

        if(strncmp(kind, "compute", UINT8_MAX) == 0) {
            shader->stage = VK_SHADER_STAGE_COMPUTE_BIT;
        } else if(strncmp(kind, "vertex", UINT8_MAX) == 0) {
            shader->stage = VK_SHADER_STAGE_VERTEX_BIT;
        } else if(strncmp(kind, "fragment", UINT8_MAX) == 0) {
            shader->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        } //TODO: Add other shader types

        if(strncmp(type, "intermediate", UINT8_MAX) == 0 || strncmp(type, "spirv", UINT8_MAX) == 0) {
            shader->intermediate = VK_TRUE;
        } else {
            shader->intermediate = VK_FALSE;
        }
    }

    // Discard rest of the file for now
    fclose(file);
}
