#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]);

size_t loadTextFile(const char *subdirectory, const char *filename, char **outData);
size_t loadBinaryFile(const char *subdirectory, const char *filename, void **outData);
