#include "draw.h"

#include "queue.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "buffer.h"
#include "element.h"

uint32_t framebufferIndex;
uint32_t swapchainImageIndex;
uint32_t frameCount;

void initializeRender() {
    framebufferIndex = 0;
    swapchainImageIndex = 0;
    frameCount = 0;
}

void render() {

}
