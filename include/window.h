#pragma once

#include "pch.h"

extern SDL_Window *window;
extern VkExtent2D extent;

extern uint32_t frameIndex;
extern float timeDelta; // In microseconds

extern vec2 mouseDelta;
extern vec3 movementInput;

extern SDL_bool resizeEvent;
extern SDL_bool quitEvent;

void *loadSystemFunction(const char *name);
SDL_bool getWindowExtensions(uint32_t *extensionCount, const char **extensionNames);
void pollEvents();

void initializeSystem();
void createWindow();
void initializeMainLoop();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
