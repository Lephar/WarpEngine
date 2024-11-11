#include "draw.h"

#include "queue.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "buffer.h"
#include "element.h"

extern VkExtent2D extent;

extern Queue graphicsQueue;

extern Swapchain swapchain;

extern FramebufferSet framebufferSet;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedBuffer;

extern uint32_t  indexCount;
extern uint32_t vertexCount;

extern VkDeviceSize   indexBufferSize;
extern VkDeviceSize  vertexBufferSize;
extern VkDeviceSize uniformBufferSize;

extern Index    *  indexBuffer;
extern Vertex   * vertexBuffer;
extern Uniform  *uniformBuffer;

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
