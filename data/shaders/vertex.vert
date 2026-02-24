#version 460 core

#extension GL_EXT_debug_printf            : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 outputPosition;
layout(location = 1) out vec4 outputTangent;
layout(location = 2) out vec3 outputNormal;
layout(location = 3) out vec2 outputTexcoord0;
layout(location = 4) out vec2 outputTexcoord1;

layout(set = 1, binding = 0) uniform Camera {
    mat4 transform;
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 cameraProperties;
};

layout(set = 2, binding = 0) uniform Primitive {
    mat4 model;
    mat4 normal;
};

struct Vertex {
    vec4 position;
    vec4 tangent;
    vec4 normal;
    vec4 texcoord; // texcoord0 and texcoord1 is packed together
};

layout(set = 5, binding = 0) readonly buffer Indices {
    uint indices[];
};

layout(set = 6, binding = 0) readonly buffer Vertices {
    Vertex vertices[];
};

layout(push_constant) uniform VertexOffset {
    int vertexOffset;
};

void main() {
    uint   index  = indices[gl_VertexIndex];
    Vertex vertex = vertices[index + vertexOffset];

    vec4 position = model * vertex.position;

    debugPrintfEXT("%d, %d, %v4f", gl_VertexIndex, index, position);

    outputPosition  = position.xyz;
    outputTangent   = vertex.tangent;
    outputNormal    = vec3(normal * vertex.normal); // NOTICE: DO NOT normalize here! Fragment interpolation changes its length. Normalize it there instead.
    outputTexcoord0 = vertex.texcoord.xy;
    outputTexcoord1 = vertex.texcoord.zw;

    gl_PointSize = 1.0f;
    gl_Position  = projectionView * position;
}
