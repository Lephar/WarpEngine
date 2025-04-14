#version 460 core

#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Scene {
    mat4 anchor;
    mat4 camera;
};

layout(set = 1, binding = 0) uniform Primitive {
    mat4 model;
};

layout(location = 0) in  vec3 inputPosition;
layout(location = 1) in  vec2 inputTexcoord;

layout(location = 0) out vec3 outputAmbientLight;
layout(location = 1) out vec3 outputPosition;
layout(location = 2) out vec2 outputTexcoord;

void main()
{
    outputAmbientLight = vec3(1.0f, 1.0f, 1.0f);
    outputPosition     = inputPosition;
    outputTexcoord     = inputTexcoord;

    gl_Position = camera * vec4(outputPosition, 1.0f);
}
