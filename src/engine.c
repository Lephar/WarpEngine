#include "engine.h"

#include "helper.h"

#include "window.h"
#include "instance.h"
#include "device.h"
#include "queue.h"
#include "surface.h"
#include "memory.h"
#include "swapchain.h"
#include "buffer.h"
#include "framebuffer.h"
#include "shader.h"

char *path;
char *name;

extern VkExtent2D extent;

void initialize(int argc, char *argv[]) {
    initializeSystem();
    createWindow();
    createInstance();
    selectPhysicalDevice();
    generateQueueDetails();
    createDevice();
    getQueues();
    createSurface();
    allocateMemories();
    createBuffers();
    createSwapchain();
    createFramebufferSet();
    createModules();
}

void loop() {
    draw(NULL);
}

void quit() {
    destroyModules();
    destroyFramebufferSet();
    destroySwapchain();
    destroyBuffers();
    freeMemories();
    destroySurface();
    clearQueues();
    destroyDevice();
    destroyInstance();
    destroyWindow();
    quitSystem();
}
