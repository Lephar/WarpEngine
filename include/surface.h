#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkPresentModeKHR presentMode;
extern VkSurfaceFormatKHR surfaceFormat;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

extern VkExtent2D surfaceExtent;

void createSurface();
VkBool32 getSurfaceSupport(VkPhysicalDevice physicalDeviceCandidate);
void generateSurfaceDetails();
void destroySurface();
