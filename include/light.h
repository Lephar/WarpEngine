#pragma once

#include "pch.h"

#define POINT_LIGHT_COUNT_HARD_LIMIT 1U<<10U

typedef struct pointLightUniform {
    mat4 transform;
    vec4 color; // NOTICE: 4th element is the intensity
} PointLightUniform, *PPointLightUniform;

typedef struct lightingUniform {
    vec3 ambientLight;
    uint32_t pointLightCount;
    PointLightUniform pointLightUniforms[POINT_LIGHT_COUNT_HARD_LIMIT];
} LightingUniform, *PLightingUniform;

extern uint32_t pointLightCountLimit;
extern LightingUniform lightingUniform;

void initializeLighting();
uint32_t loadLight(cgltf_light *lightData);
