#pragma once

#include "pch.h"

typedef uint32_t Index;

struct vertex {
    vec3 position;
    vec2 texcoord;
} typedef Vertex;

struct sceneUniform {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
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
extern VkDeviceSize dynamicUniformBufferRange;
extern VkDeviceSize framebufferUniformStride;

extern uint32_t primitiveCountLimit;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;
extern void   *uniformBuffer;

extern SceneUniform     *sceneUniform;
extern PrimitiveUniform *primitiveUniforms;

void createContentBuffers();
void loadContent();
void bindContentBuffers(VkCommandBuffer commandBuffer);
void freeContent();
