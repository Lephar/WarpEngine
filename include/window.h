#pragma once

#include "pch.h"

extern uint32_t frameIndex;
extern float timeDelta; // In microseconds

extern vec2 mouseDelta;
extern vec3 mainMovementInput;
extern vec3 freeMovementInput;

extern bool resizeEvent;
extern bool quitEvent;


PFN_vkGetInstanceProcAddr initializeSystem();
SDL_Window *createWindow(const char *windowTitle, int32_t windowWidth, int32_t windowHeight);
const char *const *getRequiredExtensions(uint32_t requiredExtensionCount);

void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();

void destroyWindow();
void quitSystem();
