#pragma once

#include "pch.h"
#include "file.h"
#include "material.h"

enum contentType {
    CAMERA,
    LIGHT,
    CUBEMAP,
    STATIONARY,
    MOVABLE,
    PORTAL
} typedef ContentType;

struct uniform {
    mat4 skyboxView;
    mat4 view;
    mat4 proj;
    mat4 skyboxCamera;
    mat4 camera;
    vec3 ambientLight;
} typedef Uniform;

void loadContent();
void freeContent();
