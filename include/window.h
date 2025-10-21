#pragma once

#include "pch.h"

extern SDL_Window *window;

extern int32_t windowWidth;
extern int32_t windowHeight;

extern uint32_t frameIndex;
extern float timeDelta; // In seconds

extern vec2 mouseDelta;
extern vec3 primaryKeyboardInput;
extern vec3 secondaryKeyboardInput;
extern vec3 joystickRotation;
extern vec3 joystickMovement;

extern bool resizeEvent;
extern bool quitEvent;

void createWindow();
void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();
void destroyWindow();
