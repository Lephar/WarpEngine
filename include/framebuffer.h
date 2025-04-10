#pragma once

#include "pch.h"

typedef struct image Image;

struct framebuffer {
    Image *depthStencil;
    Image *color;
    Image *resolve;

    VkDescriptorSet sceneDescriptorSet;
    VkDescriptorSet primitiveDescriptorSet;

    VkCommandBuffer renderCommandBuffer;
    VkCommandBuffer presentCommandBuffer;

    VkSemaphore acquireSemaphore;
    VkSemaphore drawSemaphore;
    VkSemaphore blitSemaphoreRender;
    VkSemaphore blitSemaphorePresent;

    VkFence drawFence;
    VkFence blitFence;
} typedef Framebuffer;

struct framebufferSet {
    uint32_t imageCount;

    VkSampleCountFlags sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    Framebuffer *framebuffers;
} typedef FramebufferSet;

extern const uint32_t framebufferCountLimit;

extern FramebufferSet oldFramebufferSet;
extern FramebufferSet framebufferSet;

void createFramebufferSet();
void waitFramebuffer(Framebuffer *framebuffer);
void beginFramebuffer(Framebuffer *framebuffer);
void bindFramebuffer(Framebuffer *framebuffer);
void endFramebuffer(Framebuffer *framebuffer);
void destroyFramebufferSet();
