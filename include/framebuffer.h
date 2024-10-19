#pragma once

#include "image.h"
#include "zero.h"

struct framebuffer {
    Image depthStencil;
    Image color;
    Image resolve;
} typedef Framebuffer;

struct framebufferSet {
    uint32_t framebufferImageCount;

    VkSampleCountFlags sampleCount;

    VkFormat depthStencilFormat;
    VkFormat colorFormat;
    VkFormat resolveFormat;

    Framebuffer *framebuffers;
} typedef FramebufferSet;

void createFramebufferSet();
void destroyFramebufferSet();
