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

extern uint32_t indexCount;
extern uint32_t vertexCount;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;

extern VkDeviceSize sceneUniformAlignment;
extern VkDeviceSize primitiveUniformAlignment;
extern VkDeviceSize materialUniformAlignment;

extern VkDeviceSize primitiveUniformBufferRange;
extern VkDeviceSize materialUniformBufferRange;

extern VkDeviceSize framebufferSetUniformBufferOffset;
extern VkDeviceSize framebufferUniformBufferStride;

void createContentBuffers();
void loadContent();
void updateUniforms(uint32_t framebufferIndex);
void bindContentBuffers(VkCommandBuffer commandBuffer);
void freeContent();
