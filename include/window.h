#pragma once

#include "pch.h"

void initializeSystem();
void createWindow();
void initializeMainLoop();
SDL_bool pollEvents();
void finalizeMainLoop();
void destroyWindow();
void quitSystem();
