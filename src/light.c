#include "light.h"

#include "numerics.h"
#include "logger.h"

PLightUniform       pointLightUniforms;
PLightUniform        spotLightUniforms;
PLightUniform directionalLightUniforms;
PLightUniform     ambientLightUniforms;

const PLightUniform *lightTypeReferences[] = {
    &pointLightUniforms,
    &spotLightUniforms,
    &directionalLightUniforms,
    &ambientLightUniforms,
};

const PLightUniform *sceneLight = &ambientLightUniforms;

const uint32_t            lightTypeCount = sizeof(lightTypeReferences) / sizeof(*lightTypeReferences);
const uint32_t       pointLightTypeIndex = 0;
const uint32_t        spotLightTypeIndex = 1;
const uint32_t directionalLightTypeIndex = 2;
const uint32_t     ambientLightTypeIndex = 3;
      uint32_t           lightCountLimit;

void initializeLights() {
    (*sceneLight)->color[0] = 1.0f;
    (*sceneLight)->color[1] = 0.8f;
    (*sceneLight)->color[2] = 0.6f;
    (*sceneLight)->color[3] = 0.03125f;

    (*sceneLight)->fVals[0] = 1.0f;  // Constant  Attenuation Coefficient
    (*sceneLight)->fVals[1] = 0.7f;  // Linear    Attenuation Coefficient
    (*sceneLight)->fVals[2] = 1.8f;  // Quadratic Attenuation Coefficient
    (*sceneLight)->fVals[3] = 32.0f; // Specular  Falloff     Coefficient

    (*sceneLight)->iVals[pointLightTypeIndex]       = 0; // Point       Light Count
    (*sceneLight)->iVals[spotLightTypeIndex]        = 0; // Spot        Light Count
    (*sceneLight)->iVals[directionalLightTypeIndex] = 0; // Directional Light Count
    (*sceneLight)->iVals[ambientLightTypeIndex]     = 1; // Ambient     Light Count
}

uint32_t loadLight(cgltf_light *lightData) {
    debug("\tLight: %s", lightData->name);

    if(lightData->type != cgltf_light_type_point) {
        debug("\t\tOnly point lights are supported right now, skipping...");
        return UINT32_MAX;
    }

    const uint32_t lightIndex = (*sceneLight)->iVals[pointLightTypeIndex];

    if(lightIndex >= lightCountLimit) {
        debug("\t\tLight count limit reached, skipping...");
        return UINT32_MAX;
    }

    (*sceneLight)->iVals[pointLightTypeIndex]++;

    PLightUniform light = &pointLightUniforms[lightIndex];

    glmc_vec3_copy(lightData->color, light->color);
    light->color[3] = compareFloat(lightData->intensity, 0.0f) > 0 ? lightData->intensity : 1.0f;

    debug("\t\tLight successfully loaded:");
    debug("\t\t\tColor: [%0.4f, %0.4f, %0.4f]", light->color[0], light->color[1], light->color[2]);
    debug("\t\t\tIntensity: %0.4f", light->color[3]);

    return lightIndex;
}
