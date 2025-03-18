#pragma once

#include "pch.h"

struct status {
    SDL_bool resize;
    SDL_bool quit;
} typedef Status;

extern SDL_Window *window;
extern VkExtent2D extent;
extern uint32_t frameIndex;
extern Status status;

void *loadSystemFunction(const char *name);
SDL_bool getWindowExtensions(uint32_t *extensionCount, const char **extensionNames);
void pollEvents();

void initializeSystem();
void createWindow();
void initializeMainLoop();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
