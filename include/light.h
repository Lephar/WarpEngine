#pragma once

#include "pch.h"

typedef struct sceneLightingUniform {
    vec4 ambientLight; // R,G,B,Intensity
    vec4 attenuationCoefficients; // Constant, Linear, Quadratic, Specular Falloff
    uint32_t lightTypeCounts[4]; // Point, Spot, Directional, Ambient. Only the first one is used for now, others are just padding
    uint32_t padding[4];
} SceneLightingUniform, *PSceneLightingUniform;

typedef struct lightUniform {
    mat4 transform;
    vec4 color; // R,G,B,Intensity
    vec4 extra; // Different values depending on light type
} LightUniform, *PLightUniform;

extern uint32_t lightCountLimit;
extern SceneLightingUniform sceneLightingUniform;
extern PLightUniform pointLightUniforms;
extern PLightUniform spotLightUniforms;
extern PLightUniform directionalLightUniforms;

void initializeLighting();
uint32_t loadLight(cgltf_light *lightData);
