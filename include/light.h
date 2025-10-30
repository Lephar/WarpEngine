#pragma once

#include "pch.h"

typedef struct light {
    vec3 color;
    float intensity;

    uint32_t uniformOffset;
} Light, *PLight;

typedef struct lightUniform {
    mat4 transform;
    vec4 color; // NOTICE: Fourth element is intensity
} LightUniform, *PLightUniform;

extern uint32_t lightCountLimit;
extern uint32_t lightCount;
extern PLight lights;
extern PLightUniform lightUniforms;

uint32_t loadLight(cgltf_light *lightData);