#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define  SEC_TO_MSEC  1000.0f
#define MSEC_TO_USEC  SEC_TO_MSEC
#define USEC_TO_NSEC MSEC_TO_USEC

#define EPSILON 0.000000000931322574615478515625f // 2 ^ -30

struct data {
    size_t size;
    void *content;
} typedef Data;

int32_t compareFloat(float first, float second);

float radians(float degrees);

Data wrapData(size_t size, char *content);
Data allocateData(size_t size);
void copyData(size_t size, const char *content, Data *data);
Data makeData(size_t size, const char *content);
void freeData(Data *data);
