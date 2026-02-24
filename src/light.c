#include "light.h"

#include "numerics.h"
#include "logger.h"

uint32_t pointLightCountLimit;
LightingUniform lightingUniform;

void initializeLighting() {
    lightingUniform.ambientLight[0] = 1.0f;
    lightingUniform.ambientLight[1] = 0.8f;
    lightingUniform.ambientLight[2] = 0.6f;
    lightingUniform.ambientLight[3] = 0.03125f;

    lightingUniform.attenuationCoefficients[0] = 1.0f;
    lightingUniform.attenuationCoefficients[1] = 0.7f;
    lightingUniform.attenuationCoefficients[2] = 1.8f;
    lightingUniform.attenuationCoefficients[3] = 32.0f;
}

uint32_t loadLight(cgltf_light *lightData) {
    debug("\tLight: %s", lightData->name);

    if(lightData->type != cgltf_light_type_point) {
        debug("\t\tOnly point lights are supported right now, skipping...");
        return UINT32_MAX;
    }

    if(lightingUniform.lightTypeCounts[0] >= pointLightCountLimit) {
        debug("\t\tLight count limit reached, skipping...");
        return UINT32_MAX;
    }

    const uint32_t lightIndex = lightingUniform.lightTypeCounts[0];
    lightingUniform.lightTypeCounts[0]++;

    PPointLightUniform light = &lightingUniform.pointLightUniforms[lightIndex];

    glmc_vec3_copy(lightData->color, light->color);
    light->color[3] = compareFloat(lightData->intensity, 0.0f) > 0 ? lightData->intensity : 1.0f;

    debug("\t\tLight successfully loaded:");
    debug("\t\t\tColor: [%0.4f, %0.4f, %0.4f]", light->color[0], light->color[1], light->color[2]);
    debug("\t\t\tIntensity: %0.4f", light->color[3]);

    return lightIndex;
}
