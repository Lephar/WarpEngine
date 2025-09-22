#pragma once

#include "pch.h"

extern uint32_t surfaceInstanceExtensionCount;
extern char const **surfaceInstanceExtensions;

extern uint32_t surfaceDeviceExtensionCount;
extern char const **surfaceDeviceExtensions;

extern void *surfaceDeviceFeatures;

extern VkSurfaceKHR surface;

extern VkPresentModeKHR presentMode;
extern VkSurfaceFormatKHR surfaceFormat;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

extern VkExtent2D surfaceExtent;

void createSurface();
VkBool32 getSurfaceSupport(VkPhysicalDevice physicalDeviceCandidate);
void setSurfaceDetails();
void destroySurface();
