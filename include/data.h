#pragma once

#include "pch.h"

struct data {
    size_t size;
    void *content;
} typedef Data;

Data *wrapData(size_t size, void *content);
Data *allocateData(size_t size);
void copyData(const Data *source, Data *destination);
Data *makeData(size_t size, const void *content);
void freeData(Data *data);
