#pragma once

#include "pch.h"

extern VkInstance instance;

void *loadInstanceFunction(const char *name);
void createInstance();
void destroyInstance();
