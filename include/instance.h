#pragma once

#include "pch.h"

void *loadInstanceFunction(const char *name);

VkInstance createInstance(const char *applicationName, const char *engineName, uint32_t requiredExtensionCount, const char *const *requiredExtensions, PFN_vkGetInstanceProcAddr systemFunctionLoader);
void destroyInstance();
