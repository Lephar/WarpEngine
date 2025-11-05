#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in  vec4 inputPosition;
layout(location = 1) in  vec4 inputTangent;
layout(location = 2) in  vec4 inputNormal;
layout(location = 3) in  vec2 inputTexcoord0;
layout(location = 4) in  vec2 inputTexcoord1;

layout(location = 0) out vec4 outputColor;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
    vec3 ambientLight;
    uint pointLightCount;
};

layout(set = 1, binding = 0) uniform Light {
    mat4 transform;
    vec4 lightColor;
};

layout(set = 3, binding = 0) uniform Material {
    vec4 baseColorFactor;
    vec2 metallicRoughnessFactor;
    vec3 emissiveFactor;
    float normalScale;
};

layout(set = 4, binding = 0) uniform sampler2D baseColorSampler;
layout(set = 4, binding = 1) uniform sampler2D metallicRoughnessSampler;
layout(set = 4, binding = 2) uniform sampler2D normalSampler;

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
}

void main() {
    outputColor = color();
}
