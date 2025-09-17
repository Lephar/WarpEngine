#pragma once

#include "pch.h"

extern SDL_Window *window;

extern int32_t windowWidth;
extern int32_t windowHeight;

extern uint32_t frameIndex;
extern float timeDelta; // In microseconds

extern vec2 mouseDelta;
extern vec3 mainMovementInput;
extern vec3 freeMovementInput;

extern bool resizeEvent;
extern bool quitEvent;

void initializeSystem();

void *getSystemFunctionLoader();
const char *const *getRequiredExtensions(uint32_t requiredExtensionCount);

void createWindow(const char *title, int32_t width, int32_t height);

void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();

void destroyWindow();
void quitSystem();
