#pragma once

#include "pch.h"

struct data {
    bool binary;
    size_t size;
    void *content;
} typedef Data;

Data *wrapData(bool binary, size_t size, void *content);
Data *allocateData(bool binary, size_t size);
void copyData(const Data *source, Data *destination);
Data *makeData(bool binary, size_t size, const void *content);
void freeData(Data *data);
