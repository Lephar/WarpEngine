#include "engine.h"

#include "config.h"
#include "window.h"
#include "instance.h"
#include "physicalDevice.h"
#include "surface.h"
#include "device.h"
#include "queue.h"
#include "memory.h"
#include "buffer.h"
#include "swapchain.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "content.h"
#include "shader.h"
#include "draw.h"

void initialize(int argc, char *argv[]) {
    configure(argc, argv);

    initializeSystem();
    createWindow();
    createInstance();
    selectPhysicalDevice();
    generateQueueDetails();
    createSurface();
    createDevice();
    getQueues();
    createPipeline();
    createModules();
    allocateMemories();
    createBuffers();
    createContentBuffers();
    loadContent();
    createSwapchain();
    createFramebufferSet();
}

void recreateSwapchain() {
    vkDeviceWaitIdle(device);

    destroyFramebufferSet();
    destroySwapchain();
    destroySurface();

    createSurface();
    createSwapchain();
    createFramebufferSet();

    initializeDraw();

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

    destroyFramebufferSet();
    destroySwapchain();
    freeContent();
    destroyBuffers();
    freeMemories();
    destroyPipeline();
    destroyModules();
    destroySurface();
    clearQueues();
    destroyDevice();
    destroyInstance();
    destroyWindow();
    quitSystem();
}
