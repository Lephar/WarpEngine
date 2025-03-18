#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkBool32 surfaceSupport;

extern uint32_t surfaceFormatCount;
extern VkSurfaceFormatKHR *surfaceFormats;

extern uint32_t presentModeCount;
extern VkPresentModeKHR *presentModes;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

void createSurface();
void destroySurface();
