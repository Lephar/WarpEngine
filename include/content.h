#pragma once

#include "pch.h"

typedef uint32_t Index;

typedef struct vertex {
    vec4 position;
    vec4 tangent;
    vec4 normal;
    vec4 texcoord; // texcoord0 and texcoord1 is packed together
} Vertex, *PVertex;

extern uint32_t indexCount;
extern uint32_t vertexCount;

extern Index  *indexBuffer;
extern Vertex *vertexBuffer;

extern VkDeviceSize primitiveUniformAlignment;
extern VkDeviceSize cameraUniformAlignment;
extern VkDeviceSize materialUniformAlignment;

extern VkDeviceSize primitiveUniformBufferRange;
extern VkDeviceSize cameraUniformBufferRange;
extern VkDeviceSize materialUniformBufferRange;
extern VkDeviceSize sceneLightingUniformBufferRange;
extern VkDeviceSize lightUniformBufferRange;

extern VkDeviceSize primitiveUniformBufferOffset;
extern VkDeviceSize cameraUniformBufferOffset;
extern VkDeviceSize materialUniformBufferOffset;
extern VkDeviceSize sceneLightingUniformBufferOffset;

extern VkDeviceSize framebufferUniformBufferSize;
extern VkDeviceSize framebufferSetUniformBufferSize;

void createContentBuffers();
void loadContent();
void prepareUniforms();
void loadUniformBuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void freeContent();
