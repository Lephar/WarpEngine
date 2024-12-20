#pragma once

#include "pch.h"

void resetControls();
void preprocessFrameControls();
void mouseMove(SDL_MouseMotionEvent event);
void keyDown(SDL_KeyboardEvent event);
void keyUp(SDL_KeyboardEvent event);
void postprocessFrameControls();
