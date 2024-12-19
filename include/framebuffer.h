#pragma once

#include "pch.h"

#include "image.h"

struct framebuffer {
    Image depthStencil;
    Image color;
    Image resolve;

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
    uint32_t framebufferImageCount;

    VkSampleCountFlags sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;

    Framebuffer *framebuffers;
} typedef FramebufferSet;

void createFramebufferSet();
void destroyFramebufferSet();
