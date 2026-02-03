#pragma once

#include "pch.h"

typedef struct pointLightUniform {
    mat4 transform;
    vec4 color; // NOTICE: 4th element is the intensity
} PointLightUniform, *PPointLightUniform;

typedef struct lightingUniform {
    vec3 ambientLight;
    uint32_t pointLightCount;
    // NOTICE: Lights array will be copied to this location in uniform buffer as if it was stack-allocated
    // LightUniform lightUniforms[lightCountLimit];
} LightingUniform, *PLightingUniform;

extern uint32_t pointLightCountLimit;
extern LightingUniform lightingUniform;
extern PPointLightUniform pointLightUniforms;

uint32_t loadLight(cgltf_light *lightData);