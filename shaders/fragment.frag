#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 2, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputTexcoord;

layout(location = 0) out vec4 outputColor;

void main()
{
  //outputColor = vec4(inputAmbientLight, 1.0f) * texture(textureSampler, inputTexcoord);
    outputColor = vec4(inputAmbientLight, 1.0f) * texture(textureSampler, inputTexcoord) * (1.0f - pow(gl_FragCoord.z, 128.0f)) + pow(gl_FragCoord.z, 128.0f);
}
