#pragma once

#include "pch.h"

extern uint32_t systemExtensionCount;
extern char const *const *systemExtensions;

extern SDL_Window *window;
extern VkExtent2D extent;

extern uint32_t frameIndex;
extern float timeDelta; // In microseconds

extern vec2 mouseDelta;
extern vec3 mainMovementInput;
extern vec3 freeMovementInput;

extern bool resizeEvent;
extern bool quitEvent;

void *loadSystemFunction(const char *name);
void pollEvents();

void initializeSystem();
void createWindow();
void initializeMainLoop();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
