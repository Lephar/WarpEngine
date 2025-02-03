#include "file.h"

#include "helper.h" // TODO: Remove after use

char rootPath[PATH_MAX];
char executableName[PATH_MAX];

File readFile(const char *path, FileType type) {
    File file = {
        .type = type
    };

    FILE *descriptor = fopen(path, type == FILE_TYPE_BINARY ? "rb" : "r");
    fseek(descriptor, 0, SEEK_END);
    file.size = ftell(descriptor) + (type ? 0 : 1);
    rewind(descriptor);

    file.data = malloc(file.size);
    size_t length = fread(file.data, 1, file.size, descriptor);
    assert(length + (type == FILE_TYPE_BINARY ? 0 : 1) == file.size);
    fclose(descriptor);

    // TODO: Is this necessary?
    if(type == FILE_TYPE_TEXT) {
        debug("EOF Check: %c", (file.data)[file.size - 2]);
        (file.data)[file.size - 1] = '\0';
    }

    return file;
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
