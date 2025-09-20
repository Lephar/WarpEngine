#pragma once

#include "pch.h"

extern VkSurfaceKHR surface;

extern VkPresentModeKHR presentMode;
extern VkSurfaceFormatKHR surfaceFormat;

extern VkSurfaceCapabilitiesKHR surfaceCapabilities;

extern VkExtent2D surfaceExtent;

char const **getSurfaceInstanceExtensions(uint32_t *count);
void createSurface();
VkBool32 getSurfaceSupport(VkPhysicalDevice physicalDeviceCandidate);
void setSurfaceDetails();
char const **getSurfaceDeviceExtensions(uint32_t *count);
void *getSurfaceDeviceFeatures();
void destroySurface();
