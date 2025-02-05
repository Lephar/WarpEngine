#include "file.h"

#include "helper.h" // TODO: Remove after use

extern char rootPath[];

void makeFullPath(const char relativePath[], char outFullPath[]) {
    snprintf(outFullPath, PATH_MAX, "%s/%s", rootPath, relativePath);
    debug("%s", outFullPath);
}

Data readFile(const char *path, FileType type) {
    char fullPath[PATH_MAX];
    makeFullPath(path, fullPath);

    FILE *file = fopen(fullPath, type == FILE_TYPE_BINARY ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) + (type ? 0 : 1);
    rewind(file);

    Data data = allocateData(size);

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
