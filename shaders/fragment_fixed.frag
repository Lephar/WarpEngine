#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 outputColor;

void main()
{
    outputColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
