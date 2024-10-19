#include "framebuffer.h"

#include "swapchain.h"

extern VkExtent2D extent;
extern Swapchain swapchain;

uint32_t framebufferImageCount;

VkSampleCountFlags samples;

VkFormat depthStencilFormat;
VkFormat colorFormat;
VkFormat resolveFormat;

Framebuffer *framebuffers;

void createFramebuffer(Framebuffer *framebuffer) {
    createImage(&framebuffer->depthStencil, extent, 1, samples, depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createImage(&framebuffer->color, extent, 1, samples, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    createImage(&framebuffer->resolve, extent, 1, VK_SAMPLE_COUNT_1_BIT, resolveFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
}

void createFramebuffers() {
    framebufferImageCount = 3;

    samples = VK_SAMPLE_COUNT_2_BIT;

    depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    resolveFormat = swapchain.surfaceFormat.format;

    framebuffers = malloc(framebufferImageCount * sizeof(Framebuffer));

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferImageCount; framebufferIndex++) {
        createFramebuffer(&framebuffers[framebufferIndex]);
    }
}

void destroyFramebuffer(Framebuffer *framebuffer) {
    destroyImage(&framebuffer->resolve);
    destroyImage(&framebuffer->color);
    destroyImage(&framebuffer->depthStencil);
}

void destroyFramebuffers() {
    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferImageCount; framebufferIndex++) {
        destroyFramebuffer(&framebuffers[framebufferIndex]);
    }
}