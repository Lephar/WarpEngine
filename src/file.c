#include "file.h"

#include "helper.h" // TODO: Remove after use

char rootPath[PATH_MAX];
char executableName[PATH_MAX];

Data readFile(const char *path, FileType type) {
    Data data = {};

    FILE *file = fopen(path, type == FILE_TYPE_BINARY ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    data.size = ftell(file) + (type ? 0 : 1);
    rewind(file);

    data.content = malloc(data.size);
    size_t length = fread(data.content, 1, data.size, file);
    assert(length + (type == FILE_TYPE_BINARY ? 0 : 1) == data.size);
    fclose(file);

    // TODO: Is this necessary?
    if(type == FILE_TYPE_TEXT) {
        debug("EOF Check: %c", (data.content)[data.size - 2]);
        (data.content)[data.size - 1] = '\0';
    }

    return data;
}

void readShaderFile(const char *fullPath, uint32_t binary, size_t *size, char **data) {
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
