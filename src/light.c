#include "light.h"

#include "logger.h"

uint32_t pointLightCountLimit;
LightingUniform lightingUniform;
PPointLightUniform pointLightUniforms;

uint32_t loadLight(cgltf_light *lightData) {
    debug("\tLight: %s", lightData->name);

    if(lightData->type != cgltf_light_type_point) {
        debug("\t\tOnly point lights are supported right now, skipping...");
        return UINT32_MAX;
    }

    if(lightingUniform.pointLightCount >= pointLightCountLimit) {
        debug("\t\tLight count limit reached, skipping...");
        return UINT32_MAX;
    }

    const uint32_t lightIndex = lightingUniform.pointLightCount;
    lightingUniform.pointLightCount++;

    PPointLightUniform light = &pointLightUniforms[lightIndex];

    glmc_vec3_copy(lightData->color, light->color);
    light->color[3] = lightData->intensity;

    debug("\t\tLight successfully loaded:");
    debug("\t\t\tColor: [%g, %g, %g]", light->color[0], light->color[1], light->color[2]);
    debug("\t\t\tIntensity: %g", light->color[3]);

    return lightIndex;
}
