#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct data Data;

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]);

// WARN: Free the output data!
Data *readFile(bool binary, const char *path);
