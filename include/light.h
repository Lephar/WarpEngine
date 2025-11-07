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

typedef struct sceneLightingUniform {
    vec3 ambientLight;
    uint32_t pointLightCount;
    // NOTICE: Lights array will be copied to this location in uniform buffer as if it was stack-allocated
    // LightUniform lightUniforms[lightCountLimit];
} SceneLightingUniform, *PSceneLightingUniform;

extern vec3 ambientLight;
extern uint32_t lightCountLimit;
extern uint32_t lightCount;
extern PLight lights;
extern PLightUniform lightUniforms;
extern SceneLightingUniform sceneLightingUniform;

uint32_t loadLight(cgltf_light *lightData);