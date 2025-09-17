#pragma once

#include "pch.h"

extern VkInstance instance;

void *loadInstanceFunction(const char *name);
void createInstance(const char *applicationName, const char *engineName, uint32_t requiredExtensionCount, const char *const *requiredExtensions, PFN_vkGetInstanceProcAddr systemFunctionLoader);
void destroyInstance();
