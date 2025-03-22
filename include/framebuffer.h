#pragma once

#include "pch.h"

typedef struct image Image;

struct framebuffer {
    Image *depthStencil;
    Image *color;
    Image *resolve;

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

    VkDeviceSize frameUniformBufferSize;
} typedef FramebufferSet;

extern FramebufferSet oldFramebufferSet;
extern FramebufferSet framebufferSet;

void createFramebufferSet();
void destroyFramebufferSet();
