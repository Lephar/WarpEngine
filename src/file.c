#include "file.h"

#include "config.h"
#include "data.h"

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]) {
    if(subdirectory == NULL || strncmp(subdirectory, "", PATH_MAX) == 0) {
        snprintf(outFullPath, PATH_MAX, "%s/%s", rootPath, filename);
    } else {
        snprintf(outFullPath, PATH_MAX, "%s/%s/%s", rootPath, subdirectory, filename);
    }
}

Data *readFile(const char *path, FileType type) {
    FILE *file = fopen(path, type == FILE_TYPE_BINARY ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) + (type == FILE_TYPE_BINARY ? 0 : 1);
    rewind(file);

    Data *data = allocateData(size);

    size_t length = fread(data->content, 1, data->size, file);
    assert(length + (type == FILE_TYPE_BINARY ? 0 : 1) == data->size);
    fclose(file);

    if(type == FILE_TYPE_TEXT) {
        ((char *) data->content)[data->size - 1] = '\0';
    }

    return data;
}
