#pragma once

#include "zero.h"

#include "image.h"

struct framebuffer {
    Image depthStencil;
    Image color;
    Image resolve;

    VkCommandBuffer commandBuffer;
    VkSemaphore semaphore;
    VkFence fence;
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
