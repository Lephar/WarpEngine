#pragma once

#include "pch.h"

typedef struct buffer Buffer;
typedef struct image  Image;

struct sceneUniform {
    mat4 anchor;
    mat4 camera;
} typedef SceneUniform;

struct primitiveUniform {
    mat4 transform;
} typedef PrimitiveUniform;

extern VkSampler sampler;

extern VkDescriptorSetLayout descriptorSetLayout;

extern VkDescriptorPool sceneDescriptorPool;
extern VkDescriptorPool primitiveDescriptorPool;
extern VkDescriptorPool materialDescriptorPool;

void createDescriptorSetLayout();
void createDescriptorPool(VkDescriptorType type, uint32_t count, VkDescriptorPool *outDescriptorPool);
