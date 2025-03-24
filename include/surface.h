#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkBool32 surfaceSupport;

extern VkPresentModeKHR presentMode;

extern uint32_t surfaceFormatCount;
extern VkSurfaceFormatKHR *surfaceFormats;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

void createSurface();
void destroySurface();
