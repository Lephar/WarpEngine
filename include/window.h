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

void initializeSystem();
void createWindow();
void initializeMainLoop();
void pollEvents();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
