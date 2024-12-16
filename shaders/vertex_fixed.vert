#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform Transform {
    mat4 transform;
};

layout(location = 0) in  vec3  inputPosition;

layout(location = 0) out vec3 outputPosition;

void main()
{
    outputPosition = inputPosition;

    gl_Position = transform * vec4(outputPosition, 1.0f);
}
