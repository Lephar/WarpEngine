#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexcoord;

layout(location = 0) out vec4 outputColor;

void main()
{
    outputColor = texture(textureSampler, inputTexcoord);
}
