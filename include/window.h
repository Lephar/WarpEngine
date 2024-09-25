#pragma once

#include "zero.h"

void initialize();
void createWindow(const char *name, int32_t width, int32_t height);
void createSurface();
void draw(void (*render)());
void destroySurface();
void destroyWindow();
void quit();
