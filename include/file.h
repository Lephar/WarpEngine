#pragma once

#include "pch.h"

#include "helper.h"

enum fileType {
    FILE_TYPE_TEXT,
    FILE_TYPE_BINARY
} typedef FileType;


void makeFullPath(const char *filename, const char *subdirectory, char outFullPath[]);

// WARN: Free the output data!
Data readFile(const char *path, FileType type);
