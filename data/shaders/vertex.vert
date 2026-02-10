#version 460 core

#extension GL_ARB_separate_shader_objects : enable

#define POINT_LIGHT_COUNT_LIMIT 1024

layout(location = 0) in  vec3 inputPosition;
layout(location = 1) in  vec4 inputTangent;
layout(location = 2) in  vec3 inputNormal;
layout(location = 3) in  vec2 inputTexcoord0;
layout(location = 4) in  vec2 inputTexcoord1;

layout(location = 0) out vec4 outputPosition;
layout(location = 1) out vec4 outputTangent;
layout(location = 2) out vec4 outputNormal;
layout(location = 3) out vec2 outputTexcoord0;
layout(location = 4) out vec2 outputTexcoord1;

struct PointLight {
    mat4 lightTransform;
    vec4 lightColor;
};

layout(set = 0, binding = 0) uniform Lighting {
    vec4 ambientLight;
    vec3 attenuationCoefficients;
    uint pointLightCount;
    PointLight pointLights[POINT_LIGHT_COUNT_LIMIT];
};

layout(set = 1, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 2, binding = 0) uniform Primitive {
    mat4 model;
};

void main() {
    outputPosition  = model * vec4(inputPosition, 1.0f);
    outputTangent   = inputTangent;
    outputNormal    = model * vec4(inputNormal,   0.0f); // NOTICE: DO NOT normalize here! Fragment interpolation changes its length. Normalize it there instead.
    outputTexcoord0 = inputTexcoord0;
    outputTexcoord1 = inputTexcoord1;

    gl_PointSize = 1.0f;
    gl_Position  = projectionView * outputPosition;
}
