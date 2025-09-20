#pragma once

#include "pch.h"

extern PFN_vkGetInstanceProcAddr systemFunctionLoader;
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

void createWindow();

void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();

void destroyWindow();
void quitSystem();
