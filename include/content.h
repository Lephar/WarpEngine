#pragma once

#include "pch.h"

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec4 tangent;
    vec3 normal;
    vec2 texcoord0;
    vec2 texcoord1;
} typedef Vertex;

struct sceneUniform {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraProperties;
} typedef SceneUniform;

struct primitiveUniform {
    mat4 model;
} typedef PrimitiveUniform;

extern uint32_t indexCount;
extern uint32_t vertexCount;

extern VkDeviceSize indexBufferSize;
extern VkDeviceSize vertexBufferSize;
extern VkDeviceSize uniformBufferSize;

extern VkDeviceSize sceneUniformAlignment;
extern VkDeviceSize primitiveUniformAlignment;
extern VkDeviceSize factorUniformAlignment;
extern VkDeviceSize dynamicUniformBufferRange;
extern VkDeviceSize factorUniformBufferRange;
extern VkDeviceSize framebufferUniformStride;
extern VkDeviceSize factorUniformBufferOffset;

extern uint32_t primitiveCountLimit;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;
extern void   *uniformBuffer;

extern SceneUniform *sceneUniform;

void createContentBuffers();
void loadContent();
void updateUniforms(uint32_t framebufferIndex);
void bindContentBuffers(VkCommandBuffer commandBuffer);
void freeContent();
