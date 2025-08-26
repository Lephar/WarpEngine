#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 2, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec4 inputPosition;
layout(location = 1) in vec2 inputTexcoord;

layout(location = 0) out vec4 outputColor;

vec4 position() {
    float distanceRange = 16.0f;
    return (inputPosition + distanceRange / 2.0f) / distanceRange;
}

vec4 depth() {
    float distanceFalloff = 32.0f;
    return vec4(pow(gl_FragCoord.z, distanceFalloff));
}

vec4 color() {
    return texture(textureSampler, inputTexcoord);
}

vec4 fog() {
    const float drawDistance = 128.0f;
    float normalizedHeight = (drawDistance - min(inputPosition.z, drawDistance)) / drawDistance;

    float heightFalloff = 8.0f;
    vec4 heightValue = vec4(pow(normalizedHeight, heightFalloff));

    vec4 fogDensity = depth() * heightValue;

    return color() * (vec4(1.0f) - fogDensity) + fogDensity;
}

vec4 grayscale(vec4 color) {
    float brightness = (color.r + color.g + color.b) / 3.0f;
    return vec4(vec3(brightness), 1.0f);
}

void main() {
    outputColor = color();
}
