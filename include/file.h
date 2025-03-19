#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

enum fileType {
    FILE_TYPE_TEXT,
    FILE_TYPE_BINARY
} typedef FileType;

typedef struct data Data;

void makeFullPath(const char *filename, const char *subdirectory, char outFullPath[]);

// WARN: Free the output data!
Data *readFile(const char *path, FileType type);
