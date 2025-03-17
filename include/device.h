#pragma once

#include "pch.h"

extern VkPhysicalDevice physicalDevice;
extern VkDevice device;

void selectPhysicalDevice();
void createDevice();
void destroyDevice();
