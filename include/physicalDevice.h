#pragma once

#include "pch.h"

extern VkPhysicalDevice physicalDevice;

extern VkPhysicalDeviceProperties physicalDeviceProperties;
extern VkPhysicalDeviceFeatures physicalDeviceFeatures;

extern uint32_t queueFamilyCount;
extern VkQueueFamilyProperties *queueFamilyProperties;

extern VkPhysicalDeviceMemoryProperties memoryProperties;

void selectPhysicalDevice();
void setPhysicalDeviceDetails();