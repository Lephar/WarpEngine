#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform Transform {
    mat4 skyboxView;
    mat4 view;
    mat4 proj;
    mat4 skyboxCamera;
    mat4 camera;
};

layout(location = 0) in  vec3  inputPosition;
layout(location = 1) in  vec2  inputTexcoord;

layout(location = 0) out vec3 outputPosition;
layout(location = 1) out vec2 outputTexcoord;

void main()
{
    outputPosition = inputPosition;
    outputTexcoord = inputTexcoord;

    gl_Position = camera * vec4(outputPosition, 1.0f);
}
