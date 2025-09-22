#pragma once

#include "pch.h"

void makeFullPath(const char *subdirectory, const char *filename, char outFullPath[]);
size_t loadTextFile(const char *subdirectory, const char *filename, char **outData);
