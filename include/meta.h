#pragma once

#include "pch.h"

struct camera {
    mat4 view;
    mat4 proj;
} typedef Camera;

void loadCamera(vec3 position);
