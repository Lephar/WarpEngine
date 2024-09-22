#pragma once

#include "zero.h"

void initialize();
PFN_vkGetInstanceProcAddr getLoader();
void createWindow(const char *name, int32_t width, int32_t height);
int getExtensionCount();
const char **getExtensions();
void createSurface();
void draw(void (*render)(void));
void destroySurface();
void destroyWindow();
void quit();
