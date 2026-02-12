#pragma once

#include "pch.h"

#define POINT_LIGHT_COUNT_HARD_LIMIT 1U<<10U

typedef struct pointLightUniform {
    mat4 transform;
    vec4 color; // R,G,B,Intensity
    vec4 padding;
} PointLightUniform, *PPointLightUniform;

typedef struct lightingUniform {
    vec4 ambientLight; // R,G,B,Intensity
    vec4 attenuationCoefficients; // Constant, Linear, Quadratic, Specular Falloff
    uint32_t lightTypeCounts[4]; // Point, Spot, Directional, Ambient. Only the first one is used for now, others are just padding
    uint32_t padding[4];
    PointLightUniform pointLightUniforms[POINT_LIGHT_COUNT_HARD_LIMIT];
} LightingUniform, *PLightingUniform;

extern uint32_t pointLightCountLimit;
extern LightingUniform lightingUniform;

void initializeLighting();
uint32_t loadLight(cgltf_light *lightData);
