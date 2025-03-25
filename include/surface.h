#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkBool32 surfaceSupport;

extern VkPresentModeKHR presentMode;
extern VkSurfaceFormatKHR surfaceFormat;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

void createSurface();
void destroySurface();
