#pragma once

#include "pch.h"

extern SDL_Window *window;

extern uint32_t frameIndex;

extern SDL_bool quitEvent;
extern SDL_bool resizeEvent;

void initializeSystem();
void createWindow();
void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
