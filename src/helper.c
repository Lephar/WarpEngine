#include "helper.h"

#include "window.h"
#include "instance.h"
#include "config.h"

Data wrapData(size_t size, char *content) {
    Data data = {
        .size = size,
        .content = content
    };

    return data;
}

Data allocateData(size_t size) {
    Data data = {
        .size = size,
        .content = malloc(size)
    };

    return data;
}

void copyData(size_t size, const char *content, Data *data) {
    assert(size == data->size);
    memcpy(data->content, content, size);
}

Data makeData(size_t size, const char *content) {
    Data data = allocateData(size);
    copyData(size, content, &data);
    return data;
}

void freeData(Data *data) {
    data->size = 0;
    free(data->content);
    data->content = NULL;
}
