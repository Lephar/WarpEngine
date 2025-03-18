#pragma once

#include "pch.h"

extern VkDevice device;

void *loadDeviceFunction(const char *name);

void createDevice();
void destroyDevice();
