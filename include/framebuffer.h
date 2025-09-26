#pragma once

#include "pch.h"

typedef struct image *PImage;
typedef struct camera *PCamera;

typedef struct framebuffer {
    VkDescriptorSet cameraDescriptorSet;
    VkDescriptorSet primitiveDescriptorSet;
    VkDescriptorSet materialDescriptorSet;

    VkCommandBuffer renderCommandBuffer;
    VkCommandBuffer presentCommandBuffer;

    VkSemaphore drawSemaphore;
    VkSemaphore blitSemaphore;

    VkFence drawFence;
    VkFence blitFence;

    PImage resolve;
} Framebuffer, *PFramebuffer;

typedef struct framebufferSet {
    VkExtent2D extent;
    PCamera camera;

    VkSampleCountFlagBits sampleCount;
    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    PImage depthStencil;
    PImage color;

    uint32_t framebufferCount;
    PFramebuffer framebuffers;
} FramebufferSet, *PFramebufferSet;

extern const uint32_t framebufferSetCountLimit;
extern const uint32_t framebufferSetFramebufferCountLimit;

extern uint32_t framebufferSetCount;
extern PFramebufferSet framebufferSets;

void createFramebufferSets();
void createFramebufferSet(uint32_t framebufferSetIndex);
void waitFramebufferDraw(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void waitFramebufferBlit(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void beginFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void bindFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void endFramebuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex);
void destroyFramebufferSet(uint32_t framebufferSetIndex);
void destroyFramebufferSets();
