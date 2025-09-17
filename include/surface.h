#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkPresentModeKHR presentMode;
extern VkSurfaceFormatKHR surfaceFormat;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

VkExtent2D surfaceExtent;

void createSurface();
VkBool32 getSurfaceSupport(VkPhysicalDevice physicalDevice);
void generateSurfaceDetails();
void destroySurface();
