#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
};

layout(set = 1, binding = 0) uniform Primitive {
    mat4 model;
};

layout(location = 0) in  vec3 inputPosition;
layout(location = 1) in  vec2 inputTexcoord;

layout(location = 0) out vec4 outputPosition;
layout(location = 1) out vec2 outputTexcoord;

void main() {
    outputPosition = model * vec4(inputPosition, 1.0f);
    outputTexcoord = inputTexcoord;

    gl_PointSize = 1.0f;
    gl_Position = projectionView * outputPosition;
}
