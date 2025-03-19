#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

struct data {
    size_t size;
    void *content;
} typedef Data;

Data wrapData(size_t size, char *content);
Data allocateData(size_t size);
void copyData(size_t size, const char *content, Data *data);
Data makeData(size_t size, const char *content);
void freeData(Data *data);
