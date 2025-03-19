#include "data.h"

Data *wrapData(size_t size, void *content) {
    Data *data = malloc(sizeof(Data));

    data->size = size;
    data->content = content;

    return data;
}

Data *allocateData(size_t size) {
    Data *data = malloc(sizeof(Data));

    data->size = size;
    data->content = malloc(size);

    return data;
}

void copyData(const Data *source, Data *destination) {
    assert(source->size == destination->size);

    memcpy(destination->content, source->content, source->size);
}

Data *makeData(size_t size, const void *content) {
    Data *data = allocateData(size);

    memcpy(data->content, content, size);

    return data;
}

void freeData(Data *data) {
    free(data->content);

    data->content = NULL;
    data->size = 0;

    free(data);
}
