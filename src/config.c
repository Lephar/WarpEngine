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

ShaderFile vertexShader;
ShaderFile fragmentShader;

ShaderFile *shaderReferences[] = {
    &vertexShader,
    &fragmentShader
};

uint32_t shaderCount;

extern uint32_t modelCount;
extern Model *models;

void readShaderFile(FILE *file, ShaderFile *shader) {
    char kind[UINT8_MAX];
    char type[UINT8_MAX];

    fscanf(file, "%s%s%s", shader->file.name, kind, type);

    const char *kindExtension = ".glsl";
    const char *typeExtension = "";

    if(strncmp(kind, "compute", UINT8_MAX) == 0) {
        shader->stage = VK_SHADER_STAGE_COMPUTE_BIT;
        kindExtension = ".comp";
    } else if(strncmp(kind, "vertex", UINT8_MAX) == 0) {
        shader->stage = VK_SHADER_STAGE_VERTEX_BIT;
        kindExtension = ".vert";
    } else if(strncmp(kind, "fragment", UINT8_MAX) == 0) {
        shader->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        kindExtension = ".frag";
    } //TODO: Add other shader types

    if(strncmp(type, "intermediate", UINT8_MAX) == 0 || strncmp(type, "spirv", UINT8_MAX) == 0) {
        shader->intermediate = VK_TRUE;
        typeExtension = ".spv";
    } else {
        shader->intermediate = VK_FALSE;
    }

    sprintf(shader->file.fullpath, "%s/shaders/%s", rootPath, shader->file.name);

    debug("Shader: %s", shader->file.name);
    debug("\tKind: %s", kind);
    debug("\tType: %s", type);
}

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
        readShaderFile(shaderReferences[shaderIndex]);
    }

    fscanf(file, "%s", discard);
    assert(strncmp(discard, "Assets:", PATH_MAX) == 0);

    fscanf(file, "%u", &modelCount);
    debug("Asset count: %d", modelCount);

    models = malloc(modelCount * sizeof(Model)); // NOTICE: Free in the content unit

    for(uint32_t modelIndex = 0; modelIndex < modelCount; modelIndex++) {
        Model *model = &models[modelIndex];

        char type[UINT8_MAX];

        fscanf(file, "%s%s", model->name, type);

        if(strncmp(type, "binary", PATH_MAX) == 0) {
            model->binary = VK_TRUE;
        } else {
            model->binary = VK_FALSE;
        }

        length = snprintf(model->fullpath, PATH_MAX, "%s/assets/%s.%s", rootPath, model->name, model->binary ? "glb" : "gltf");
        assert(length < PATH_MAX);
        debug("\t%s", model->fullpath);
    }

    fclose(file);
}
