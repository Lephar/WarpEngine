#include "framebuffer.h"

#include "helper.h"
#include "memory.h"
#include "swapchain.h"

extern VkExtent2D extent;
extern Swapchain swapchain;
extern Memory deviceMemory;

FramebufferSet framebufferSet;
FramebufferSet oldFramebufferSet;

void createFramebuffer(Framebuffer *framebuffer) {
    createImage(&framebuffer->depthStencil, extent.width, extent.height, 1, framebufferSet.sampleCount, framebufferSet.depthStencilFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    createImage(&framebuffer->color, extent.width, extent.height, 1, framebufferSet.sampleCount, framebufferSet.colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    createImage(&framebuffer->resolve, extent.width, extent.height, 1, VK_SAMPLE_COUNT_1_BIT, framebufferSet.resolveFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

    bindImageMemory(&framebuffer->depthStencil, &deviceMemory);
    bindImageMemory(&framebuffer->color, &deviceMemory);
    bindImageMemory(&framebuffer->resolve, &deviceMemory);

    createImageView(&framebuffer->depthStencil, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
    createImageView(&framebuffer->color, VK_IMAGE_ASPECT_COLOR_BIT);
    createImageView(&framebuffer->resolve, VK_IMAGE_ASPECT_COLOR_BIT);
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
        debug("Framebuffer %d created", framebufferIndex);
    }
}

void destroyFramebuffer(Framebuffer *framebuffer) {
    destroyImageView(&framebuffer->resolve);
    destroyImageView(&framebuffer->color);
    destroyImageView(&framebuffer->depthStencil);

    destroyImage(&framebuffer->resolve);
    destroyImage(&framebuffer->color);
    destroyImage(&framebuffer->depthStencil);
}

void destroyFramebufferSet() {
    for(uint32_t framebufferIndex = 0; framebufferIndex < framebufferSet.framebufferImageCount; framebufferIndex++) {
        destroyFramebuffer(&framebufferSet.framebuffers[framebufferIndex]);
        debug("Framebuffer %d destroyed", framebufferIndex);
    }
}