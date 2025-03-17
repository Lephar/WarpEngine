#pragma once

#include "pch.h"

extern SDL_bool windowCreated;
extern SDL_Window *window;

extern uint32_t frameIndex;

extern SDL_bool quitEvent;
extern SDL_bool resizeEvent;

extern uint32_t requiredInstanceExtensionCount;
extern const char **requiredInstanceExtensionNames;

extern PFN_vkGetInstanceProcAddr getInstanceProcAddr;

void initializeSystem();
void createWindow();
void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
