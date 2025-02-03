#include "file.h"

#include "helper.h" // TODO: Remove after use

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
