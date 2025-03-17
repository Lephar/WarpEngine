#include "engine.h"

#include "helper.h"
#include "config.h"

#include "window.h"
#include "instance.h"
#include "device.h"
#include "queue.h"
#include "surface.h"
#include "memory.h"
#include "swapchain.h"
#include "buffer.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "shader.h"
#include "content.h"

#include "draw.h"

void initialize(int argc, char *argv[]) {
    configure(argc, argv);

    initializeSystem();
    createWindow();
    createInstance();
    selectPhysicalDevice();
    generateQueueDetails();
    createDevice();
    getQueues();
    createSurface();
    createSampler();
    createLayouts();
    createDescriptorPool();
    createModules();
    allocateMemories();
    createBuffers();
    loadContent();
    createSwapchain();
    createFramebufferSet();

    vkDeviceWaitIdle(device);
}

void recreateSwapchain() {
    vkDeviceWaitIdle(device);

    destroyFramebufferSet();
    destroySwapchain();
    destroySurface();

    createSurface();
    createSwapchain();
    createFramebufferSet();

    vkDeviceWaitIdle(device);

    resizeEvent = SDL_FALSE;
}

void loop() {
    initializeMainLoop();
    initializeDraw();

    while(1) {
        pollEvents();

        if(quitEvent) {
            break;
        } else if(resizeEvent) {
            recreateSwapchain();
        }

        draw();
    }

    finalizeDraw();
    finalizeMainLoop();
}

void quit() {
    vkDeviceWaitIdle(device);

    destroyModules();
    destroyPipeline();
    destroyFramebufferSet();
    destroySwapchain();
    destroyBuffers();
    freeContent();
    freeMemories();
    destroySurface();
    clearQueues();
    destroyDevice();
    destroyInstance();
    destroyWindow();
    quitSystem();
}
