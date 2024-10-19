#include "framebuffer.h"

#include "swapchain.h"

extern VkExtent2D extent;
extern Swapchain swapchain;

FramebufferSet framebufferSet;
FramebufferSet oldFramebufferSet;

void createFramebuffer(Framebuffer *framebuffer) {
    createImage(&framebuffer->depthStencil, extent, 1, framebufferSet.sampleCount, framebufferSet.depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createImage(&framebuffer->color, extent, 1, framebufferSet.sampleCount, framebufferSet.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    createImage(&framebuffer->resolve, extent, 1, VK_SAMPLE_COUNT_1_BIT, framebufferSet.resolveFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
}

void createFramebufferSet() {
    framebufferSet.framebufferImageCount = 3;

    framebufferSet.sampleCount = VK_SAMPLE_COUNT_2_BIT;

    framebufferSet.depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    framebufferSet.colorFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    framebufferSet.resolveFormat = swapchain.surfaceFormat.format;

    framebufferSet.framebuffers = malloc(framebufferSet.framebufferImageCount * sizeof(Framebuffer));

    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.framebufferImageCount; framebufferIndex++) {
        createFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
    }
}

void destroyFramebuffer(Framebuffer *framebuffer) {
    destroyImage(&framebuffer->resolve);
    destroyImage(&framebuffer->color);
    destroyImage(&framebuffer->depthStencil);
}

void destroyFramebufferSet() {
    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.framebufferImageCount; framebufferIndex++) {
        destroyFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
    }
}