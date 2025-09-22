#pragma once

#include "pch.h"

extern int32_t threadCount;

extern char const *applicationName;
extern char dataPath[];

extern PFN_vkGetInstanceProcAddr systemFunctionLoader;

void initializeSystem();
void quitSystem();