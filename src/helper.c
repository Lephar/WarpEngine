#include "helper.h"

#include "window.h"
#include "instance.h"
#include "config.h"

void debug(const char *fmt, ...) {
    if(DEBUG) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }
}

void *loadFunction(const char *name) {
    void  *functionPointer = getInstanceProcAddr(instance, name);
    assert(functionPointer);
    return functionPointer;
}

int32_t compareFloat(float first, float second) {
    float difference = first - second;

    return (difference > EPSILON) - (difference < -EPSILON);
}

float radians(float degrees) {
    return M_PI * degrees / 180.0f;
}

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
