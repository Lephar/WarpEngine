#pragma once

#include "pch.h"

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

extern char rootPath[];
extern char executableName[];
extern const char *dataDirectory;

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]);
size_t loadTextFile(const char *subdirectory, const char *filename, char **outData);
