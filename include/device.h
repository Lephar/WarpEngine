#pragma once

#include "pch.h"

extern VkPhysicalDevice physicalDevice;
extern VkDevice device;

void *loadDeviceFunction(const char *name);

void selectPhysicalDevice();
void createDevice();
void destroyDevice();
