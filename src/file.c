#include "file.h"

char rootPath[PATH_MAX];
char executableName[PATH_MAX];
const char *dataDirectory = "data";

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]) {
    int length = 0;

    if(subdirectory == nullptr || strncmp(subdirectory, "", PATH_MAX) == 0) {
        length = snprintf(outFullPath, PATH_MAX, "%s/%s/%s",    rootPath, dataDirectory, filename);
    } else {
        length = snprintf(outFullPath, PATH_MAX, "%s/%s/%s/%s", rootPath, dataDirectory, subdirectory, filename);
    }

    assert(length < PATH_MAX);
}

size_t loadTextFile(const char *subdirectory, const char *filename, char **outData) {
    char fullPath[PATH_MAX];
    makeFullPath(subdirectory, filename, fullPath);

    FILE *file = fopen(fullPath, "r");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    if(*outData == nullptr) {
        *outData = malloc(size + 1);
    }

    size_t length = fread(*outData, 1, size, file);
    assert(length == size);

    (*outData)[size] = '\0';
    fclose(file);

    return size;
}
