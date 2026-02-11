#version 460 core

#extension GL_ARB_separate_shader_objects : enable

#define POINT_LIGHT_COUNT_LIMIT 1024

layout(location = 0) in  vec4 inputPosition;
layout(location = 1) in  vec4 inputTangent;
layout(location = 2) in  vec4 inputNormal;
layout(location = 3) in  vec2 inputTexcoord0;
layout(location = 4) in  vec2 inputTexcoord1;

layout(location = 0) out vec4 outputColor;

struct PointLight {
    mat4 lightTransform;
    vec4 lightColor;
};

layout(set = 0, binding = 0) uniform Lighting {
    vec4 ambientLight;
    vec4 attenuationCoefficients;
    uint pointLightCount;
    PointLight pointLights[POINT_LIGHT_COUNT_LIMIT];
};

layout(set = 1, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 3, binding = 0) uniform Material {
    vec4 baseColorFactor;
    vec4 metallicRoughnessFactor;
    vec4 emissiveFactor;
    float occlusionScale;
    float normalScale;
};

layout(set = 4, binding = 0) uniform sampler2D baseColorSampler;
layout(set = 4, binding = 1) uniform sampler2D metallicRoughnessSampler;
layout(set = 4, binding = 2) uniform sampler2D emissiveSampler;
layout(set = 4, binding = 3) uniform sampler2D occlusionSampler;
layout(set = 4, binding = 4) uniform sampler2D normalSampler;

vec4 depth() {
    float nearPlane = cameraProperties[2];
    float farPlane  = cameraProperties[3];

    float linearisedDepthValue = farPlane * nearPlane / (farPlane - gl_FragCoord.z * (farPlane - nearPlane));
    float normalisedDepthValue = linearisedDepthValue / farPlane;

    return vec4(vec3(normalisedDepthValue), 1.0f);
}

vec4 color() {
    return baseColorFactor * texture(baseColorSampler, inputTexcoord0);
}

vec4 metallicRoughness() {
    vec4 metallicRoughnessValue = texture(metallicRoughnessSampler, inputTexcoord0);

    return vec4(metallicRoughnessValue.r, metallicRoughnessFactor.y * metallicRoughnessValue.g, metallicRoughnessFactor.x * metallicRoughnessValue.b, metallicRoughnessValue.a);
}

vec4 normal() {
    return texture(normalSampler, inputTexcoord0);
vec3 emissive() {
    return emissiveFactor.rgb;// * texture(emissiveSampler, inputTexcoord0).rgb;
}

vec3 occlusion() {
    return vec3(occlusionScale * texture(occlusionSampler, inputTexcoord0).r);
}

}

vec3 pointLightDiffuse(uint pointLightIndex) {
    vec3  lightColor     = vec3(pointLights[pointLightIndex].lightColor);
    vec4  lightPosition  = pointLights[pointLightIndex].lightTransform * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4  lightVector    = lightPosition - inputPosition;
    vec4  lightDirection = normalize(lightVector);
    float lightDistance  = length(lightVector);

    float Kc = attenuationCoefficients[0];
    float Kl = attenuationCoefficients[1];
    float Kq = attenuationCoefficients[2];

    float lightAttenuation = 1.0f / (Kc + Kl * lightDistance + Kq * lightDistance * lightDistance);
    float lightIntensity   = pointLights[pointLightIndex].lightColor[3];
    float lightImpact      = lightIntensity * lightAttenuation;

    float lightDiffuse = max(dot(vec3(normalize(inputNormal)), vec3(lightDirection)), 0.0f);

    return lightImpact * lightDiffuse * lightColor;
}

vec3 pointLightSpecular(uint pointLightIndex) {
    vec4  viewPosition   = inverse(view) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4  viewVector     = viewPosition - inputPosition;
    vec4  viewDirection  = normalize(viewVector);

    vec3  lightColor     = vec3(pointLights[pointLightIndex].lightColor);
    vec4  lightPosition  = pointLights[pointLightIndex].lightTransform * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4  lightVector    = lightPosition - inputPosition;
    vec4  lightDirection = normalize(lightVector);
    float lightDistance  = length(lightVector);

    float Kc = attenuationCoefficients[0];
    float Kl = attenuationCoefficients[1];
    float Kq = attenuationCoefficients[2];

    float lightAttenuation = 1.0f / (Kc + Kl * lightDistance + Kq * lightDistance * lightDistance);
    float lightIntensity   = pointLights[pointLightIndex].lightColor[3];
    float lightImpact      = lightIntensity * lightAttenuation;

    float specularFalloff  = attenuationCoefficients[3];
    vec4  halfwayDirection = normalize(viewDirection + lightDirection);
    float lightSpecular    = pow(max(dot(normalize(inputNormal), halfwayDirection), 0.0f), specularFalloff);

    return lightImpact * lightSpecular * lightColor;
}

void main() {
    vec3 diffuse  = vec3(0.0f, 0.0f, 0.0f);
    vec3 specular = vec3(0.0f, 0.0f, 0.0f);

    for(int pointLightIndex = 1; pointLightIndex < pointLightCount; pointLightIndex++) {
        diffuse  += pointLightDiffuse(pointLightIndex);
        specular += pointLightSpecular(pointLightIndex);
    }

    outputColor = vec4(vec3(ambientLight) + diffuse + specular, 1.0f) * color();
}
