#pragma once

#include "pch.h"

void initializeWorld(const vec3 up);
void initializePlayer(vec3 position, vec3 direction, float speed);

void updatePlayer();
