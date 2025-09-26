#pragma once

#include "pch.h"

extern SDL_Window *window;

extern int32_t windowWidth;
extern int32_t windowHeight;

extern uint32_t frameIndex;
extern float timeDelta; // In microseconds

extern vec2 mouseDelta;
extern vec3 secondaryKeyboardInput;
extern vec3 primaryKeyboardInput;

extern bool resizeEvent;
extern bool quitEvent;

void createWindow();
void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();
void destroyWindow();
