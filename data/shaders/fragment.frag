#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 2, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexcoord;

layout(location = 0) out vec4 outputColor;

vec4 depth(float distanceFalloff) {
    return vec4(pow(gl_FragCoord.z, distanceFalloff));
}

vec4 fog(vec4 color, float distanceFalloff, float heightFalloff) {
    const float drawDistance = 1000.0f;

    float normalizedHeight = (drawDistance - min(inputPosition.z, drawDistance)) / drawDistance;

    vec4 depthValue = depth(distanceFalloff);
    vec4 heightValue = vec4(pow(normalizedHeight, heightFalloff));

    vec4 fogDensity = depthValue * heightValue;

    return color * (vec4(1.0f) - fogDensity) + fogDensity;
}

vec4 grayscale(vec4 color) {
    float average = (color.r + color.g + color.b) / 3.0f;
    return vec4(average);
}

void main() {
    float distanceFalloff = 32.0f;
    float heightFalloff = 8.0f;

    vec4 color = texture(textureSampler, inputTexcoord);

    outputColor = color;
  //outputColor = depth(distanceFalloff);
  //outputColor = fog(grayscale(color), distanceFalloff, heightFalloff);
}
