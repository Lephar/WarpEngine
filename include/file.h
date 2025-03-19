#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct data Data;

// TODO: Move this into Data
enum fileType {
    FILE_TYPE_TEXT,
    FILE_TYPE_BINARY
} typedef FileType;

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]);

// WARN: Free the output data!
Data *readFile(const char *path, FileType type);
