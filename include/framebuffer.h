#pragma once

#include "pch.h"

typedef struct image Image;

typedef struct framebuffer {
    Image *resolve;

    VkDescriptorSet sceneDescriptorSet;
    VkDescriptorSet primitiveDescriptorSet;

    VkCommandBuffer renderCommandBuffer;
    VkCommandBuffer presentCommandBuffer;

    VkSemaphore drawSemaphore;
    VkSemaphore blitSemaphore;

    VkFence drawFence;
    VkFence blitFence;
} Framebuffer, *PFramebuffer;

typedef struct framebufferSet {
    uint32_t imageCount;

    VkSampleCountFlagBits sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    Image *depthStencil;
    Image *color;

    Framebuffer *framebuffers;
} FramebufferSet, *PFramebufferSet;

extern const uint32_t framebufferSetCountLimit;
extern const uint32_t framebufferSetFramebufferCountLimit;

extern FramebufferSet oldFramebufferSet;
extern FramebufferSet framebufferSet;

void createFramebufferSet();
void waitFramebufferDraw(Framebuffer *framebuffer);
void waitFramebufferBlit(Framebuffer *framebuffer);
void beginFramebuffer(Framebuffer *framebuffer);
void bindFramebuffer(Framebuffer *framebuffer);
void endFramebuffer(Framebuffer *framebuffer);
void destroyFramebufferSet();
