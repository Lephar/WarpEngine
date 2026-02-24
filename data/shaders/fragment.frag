#version 460 core

#extension GL_ARB_separate_shader_objects : enable

#define POINT_LIGHT_COUNT_LIMIT 1024

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec4 inputTangent;
layout(location = 2) in vec3 inputNormal;
layout(location = 3) in vec2 inputTexcoord0;
layout(location = 4) in vec2 inputTexcoord1;

struct PointLight {
    mat4 lightTransform;
    vec4 lightColor;
    vec4 lightPadding;
};

layout(set = 0, binding = 0) uniform Lighting {
    vec4 ambientLight;
    vec4 attenuationCoefficients;
    uvec4 lightTypeCounts;
    uvec4 lightingPadding;
    PointLight pointLights[POINT_LIGHT_COUNT_LIMIT];
};

layout(set = 1, binding = 0) uniform Camera {
    mat4 transform;
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 3, binding = 0) uniform Material {
    vec4 baseColorFactor;
    vec4 occlusionMetallicRoughnessNormalFactor;
    vec3 emissiveFactor;
};

layout(set = 4, binding = 0) uniform sampler2D baseColorSampler;
layout(set = 4, binding = 1) uniform sampler2D metallicRoughnessSampler;
layout(set = 4, binding = 2) uniform sampler2D emissiveSampler;
layout(set = 4, binding = 3) uniform sampler2D occlusionSampler;
layout(set = 4, binding = 4) uniform sampler2D normalSampler;

layout(location = 0) out vec4 outputColor;

vec4 depth() {
    float nearPlane = cameraProperties[2];
    float farPlane  = cameraProperties[3];

    float linearisedDepthValue = farPlane * nearPlane / (farPlane - gl_FragCoord.z * (farPlane - nearPlane));
    float normalisedDepthValue = linearisedDepthValue / farPlane;

    return vec4(vec3(normalisedDepthValue), 1.0f);
}

float alpha() {
    return baseColorFactor.a * texture(baseColorSampler, inputTexcoord0).a;
}

vec3 color() {
    return baseColorFactor.rgb * texture(baseColorSampler, inputTexcoord0).rgb;
}

vec3 metallicRoughness() {
    return vec3(0.0f, occlusionMetallicRoughnessNormalFactor.gb * texture(metallicRoughnessSampler, inputTexcoord0).gb);
}

vec3 emissive() {
    return emissiveFactor * texture(emissiveSampler, inputTexcoord0).rgb;
}

vec3 occlusion() {
    return vec3(occlusionMetallicRoughnessNormalFactor.r * texture(occlusionSampler, inputTexcoord0).r);
}

vec3 normal() {
    return occlusionMetallicRoughnessNormalFactor.a * texture(normalSampler, inputTexcoord0).rgb;
}

void main() {
    vec3  ambient  = ambientLight.a * ambientLight.rgb;
    vec3  diffuse  = vec3(0.0f, 0.0f, 0.0f);
    vec3  specular = vec3(0.0f, 0.0f, 0.0f);
    vec3  emissive = emissive();
    vec3  color    = color();
    float alpha    = alpha();

    vec4 viewPosition  = transform * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec3 viewVector    = viewPosition.xyz - inputPosition;
    vec3 viewDirection = normalize(viewVector);

    for(int pointLightIndex = 0; pointLightIndex < lightTypeCounts[0]; pointLightIndex++) {
        PointLight pointLight = pointLights[pointLightIndex];

        vec3  lightColor     = pointLight.lightColor.rgb;
        vec4  lightPosition  = pointLight.lightTransform * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec3  lightVector    = lightPosition.xyz - inputPosition;
        vec3  lightDirection = normalize(lightVector);
        float lightDistance  = length(lightVector);

        vec3  halfwayDirection = normalize(viewDirection + lightDirection);

        float Kc = attenuationCoefficients[0];
        float Kl = attenuationCoefficients[1];
        float Kq = attenuationCoefficients[2];

        float specularFalloff = attenuationCoefficients[3];

        float attenuation = Kc + Kl * lightDistance + Kq * lightDistance * lightDistance;
        float intensity   = pointLight.lightColor.a;
        float impact      = intensity / attenuation;

        float lightSpecular = pow(max(dot(normalize(inputNormal), halfwayDirection), 0.0f), specularFalloff);
        float lightDiffuse  = max(dot(normalize(inputNormal), lightDirection), 0.0f);

        diffuse  += impact * lightDiffuse  * lightColor;
        specular += impact * lightSpecular * lightColor;
    }

    outputColor = vec4((ambient + diffuse + specular + emissive) * color, alpha);
}
