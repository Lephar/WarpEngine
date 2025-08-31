#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 2, binding = 0) uniform Factor {
    vec4 baseColorFactor;
    vec2 metallicRoughnessFactor;
};

layout(set = 3, binding = 0) uniform sampler2D baseColorSampler;
layout(set = 3, binding = 1) uniform sampler2D metallicRoughnessSampler;
layout(set = 3, binding = 2) uniform sampler2D normalSampler;

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec2 inputTexcoord0;
layout(location = 2) in vec2 inputTexcoord1;

layout(location = 0) out vec4 outputColor;

vec4 depth() {
    float farPlane  = cameraProperties[2];
    float nearPlane = cameraProperties[1];
    float linearisedDepthValue = farPlane * nearPlane / (farPlane - gl_FragCoord.z * (farPlane - nearPlane));
    float normalisedDepthValue = linearisedDepthValue / farPlane;

    return vec4(vec3(normalisedDepthValue), 1.0f);
}

vec4 color() {
    return baseColorFactor * texture(baseColorSampler, inputTexcoord0);
}

vec4 metallicRoughness() {
    return vec4(metallicRoughnessFactor * texture(metallicRoughnessSampler, inputTexcoord0).rg, 0.0f, 1.0f);
}

vec4 normal() {
    return vec4(texture(normalSampler, inputTexcoord0).rgb, 1.0f);
}

void main() {
    outputColor = normal();
}
