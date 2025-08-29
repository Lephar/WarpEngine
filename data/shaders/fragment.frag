#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 2, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec2 inputTexcoord;

layout(location = 0) out vec4 outputColor;

vec4 position() {
    float distanceRange = 16.0f;
    return vec4((vec3(inputPosition) + distanceRange / 2.0f) / distanceRange, 1.0f);
}

vec4 depth() {
    float farPlane  = cameraProperties[2];
    float nearPlane = cameraProperties[1];
    float linearisedDepthValue = farPlane * nearPlane / (farPlane - gl_FragCoord.z * (farPlane - nearPlane));
    float normalisedDepthValue = linearisedDepthValue / farPlane;

    return vec4(vec3(normalisedDepthValue), 1.0f);
}

vec4 color() {
    return texture(textureSampler, inputTexcoord);
}

vec4 fog() {
    const float drawDistance = 128.0f;
    float normalizedHeight = (drawDistance - min(inputPosition.z, drawDistance)) / drawDistance;

    float heightFalloff = 8.0f;
    vec3 heightValue = vec3(pow(normalizedHeight, heightFalloff));

    vec3 fogDensity = vec3(depth()) * heightValue;

    return vec4(vec3(color()) * (vec3(1.0f) - fogDensity) + fogDensity, 1.0f);
}

vec4 grayscale(vec4 color) {
    float brightness = (color.r + color.g + color.b) / 3.0f;
    return vec4(vec3(brightness), 1.0f);
}

void main() {
    outputColor = color();
}
