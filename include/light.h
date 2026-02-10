#pragma once

#include "pch.h"

#define POINT_LIGHT_COUNT_HARD_LIMIT 1U<<10U

typedef struct pointLightUniform {
    mat4 transform;
    vec4 color; // R,G,B,Intensity
} PointLightUniform, *PPointLightUniform;

typedef struct lightingUniform {
    vec4 ambientLight; // R,G,B,Intensity
    vec3 attenuationCoefficients; // Constant, Linear, Quadratic
    uint32_t pointLightCount;
    PointLightUniform pointLightUniforms[POINT_LIGHT_COUNT_HARD_LIMIT];
} LightingUniform, *PLightingUniform;

extern uint32_t pointLightCountLimit;
extern LightingUniform lightingUniform;

void initializeLighting();
uint32_t loadLight(cgltf_light *lightData);
