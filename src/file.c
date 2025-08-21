#include "file.h"

#include "config.h"

const char *dataDirectory = "data";

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]) {
    if(subdirectory == NULL || strncmp(subdirectory, "", PATH_MAX) == 0) {
        snprintf(outFullPath, PATH_MAX, "%s/%s/%s",    rootPath, dataDirectory, filename);
    } else {
        snprintf(outFullPath, PATH_MAX, "%s/%s/%s/%s", rootPath, dataDirectory, subdirectory, filename);
    }
}

size_t loadTextFile(const char *subdirectory, const char *filename, char **outData) {
    char fullPath[PATH_MAX];
    makeFullPath(subdirectory, filename, fullPath);

    FILE *file = fopen(fullPath, "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    if(*outData == NULL) {
        *outData = malloc(size + 1);
    }

    size_t length = fread(*outData, 1, size, file);
    assert(length == size);

    (*outData)[size] = '\0';
    fclose(file);

    return size;
}

size_t loadBinaryFile(const char *subdirectory, const char *filename, void **outData) {
    char fullPath[PATH_MAX];
    makeFullPath(subdirectory, filename, fullPath);

    FILE *file = fopen(fullPath, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    if(*outData == NULL) {
        *outData = malloc(size);
    }

    size_t length = fread(*outData, size, 1, file);
    assert(length == size);
    fclose(file);

    return size;
}
