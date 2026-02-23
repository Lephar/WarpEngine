#version 460 core

#extension GL_EXT_debug_printf            : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 outputPosition;
layout(location = 1) out vec4 outputTangent;
layout(location = 2) out vec3 outputNormal;
layout(location = 3) out vec2 outputTexcoord0;
layout(location = 4) out vec2 outputTexcoord1;

layout(set = 1, binding = 0, std140) uniform Camera {
    mat4 transform;
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 2, binding = 0, std140) uniform Primitive {
    mat4 model;
    mat4 normal;
};

struct Vertex {
    vec4 position;
    vec4 tangent;
    vec4 normal;
    vec4 texcoord; // texcoord0 and texcoord1 is packed together
};

void main() {
    vec4 position = model * vec4(inputPosition, 1.0f);

    outputPosition  = position.xyz;
    outputTangent   = inputTangent;
    outputNormal    = vec3(normal * vec4(inputNormal, 0.0f)); // NOTICE: DO NOT normalize here! Fragment interpolation changes its length. Normalize it there instead.
    outputTexcoord0 = inputTexcoord0;
    outputTexcoord1 = inputTexcoord1;

    gl_PointSize = 1.0f;
    gl_Position  = projectionView * position;
}
