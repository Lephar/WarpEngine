#pragma once

#include "pch.h"

typedef struct image Image;

struct framebuffer {
    Image *resolve;

    VkDescriptorSet sceneDescriptorSet;
    VkDescriptorSet primitiveDescriptorSet;

    VkCommandBuffer renderCommandBuffer;
    VkCommandBuffer presentCommandBuffer;

    VkSemaphore drawSemaphore;
    VkSemaphore blitSemaphore;
    VkSemaphore acquireSemaphore;

    VkFence drawFence;
    VkFence blitFence;
} typedef Framebuffer;

struct framebufferSet {
    uint32_t imageCount;

    VkSampleCountFlagBits sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    Image *depthStencil;
    Image *color;

    Framebuffer *framebuffers;
} typedef FramebufferSet;

extern const uint32_t framebufferCountLimit;

extern FramebufferSet oldFramebufferSet;
extern FramebufferSet framebufferSet;

void createFramebufferSet();
void waitFramebufferDraw(Framebuffer *framebuffer);
void waitFramebufferBlit(Framebuffer *framebuffer);
void beginFramebuffer(FramebufferSet *framebufferSet, Framebuffer *framebuffer);
void bindFramebuffer(Framebuffer *framebuffer);
void endFramebuffer(Framebuffer *framebuffer);
void destroyFramebufferSet();
