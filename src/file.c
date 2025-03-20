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

Data *readFile(bool binary, const char *path) {
    FILE *file = fopen(path, binary ? "rb" : "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file) + !binary;
    rewind(file);

    Data *data = allocateData(binary, size);

    size_t length = fread(data->content, 1, data->size, file);
    assert(length + !binary == data->size);
    fclose(file);

    if(!binary) {
        ((char *) data->content)[data->size - 1] = '\0';
    }

    return data;
}
