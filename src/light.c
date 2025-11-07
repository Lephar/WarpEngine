#include "light.h"

#include "content.h"

#include "logger.h"

vec3 ambientLight;
uint32_t lightCountLimit;
uint32_t lightCount;
PLight lights;
PLightUniform lightUniforms;
SceneLightingUniform sceneLightingUniform;

uint32_t loadLight(cgltf_light *lightData) {
    debug("\tLight: %s", lightData->name);

    if(lightData->type != cgltf_light_type_point) {
        debug("\t\tOnly point lights are supported right now, skipping...");
        return UINT32_MAX;
    }

    if(lightCount >= lightCountLimit) {
        debug("\t\tLight count limit reached, skipping...");
        return UINT32_MAX;
    }

    const uint32_t lightIndex = lightCount;
    lightCount++;

    PLight light = &lights[lightIndex];
    PLightUniform lightUniform = &lightUniforms[lightIndex];

    light->uniformOffset = lightIndex * lightUniformAlignment;

    glmc_vec3_copy(lightData->color, light->color);
    light->intensity = lightData->intensity;

    debug("\t\tColor: [%g, %g, %g]", light->color[0], light->color[1], light->color[2]);
    debug("\t\tIntensity: %g", light->intensity);

    glmc_vec3_copy(light->color, lightUniform->color);
    lightUniform->color[3] = light->intensity;

    debug("\t\tLight successfully loaded");

    return lightIndex;
}
