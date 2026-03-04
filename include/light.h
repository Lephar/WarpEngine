#pragma once

#include "pch.h"

typedef struct lightUniform {
    mat4  transform;
    vec4  color; // R,G,B,Intensity
    vec4  extra; // Different integer values depending on light type
    vec4  fVals; // Different float   values depending on light type
    ivec4 iVals; // Different integer values depending on light type
} LightUniform, *PLightUniform;

extern const uint32_t            lightTypeCount;
extern const uint32_t       pointLightTypeIndex;
extern const uint32_t        spotLightTypeIndex;
extern const uint32_t directionalLightTypeIndex;
extern const uint32_t     ambientLightTypeIndex;
extern       uint32_t           lightCountLimit;

extern PLightUniform       pointLightUniforms;
extern PLightUniform        spotLightUniforms;
extern PLightUniform directionalLightUniforms;
extern PLightUniform     ambientLightUniforms;

extern PLightUniform lightTypeReferences[];
extern PLightUniform sceneLight;

void initializeLights();
uint32_t loadLight(cgltf_light *lightData);
