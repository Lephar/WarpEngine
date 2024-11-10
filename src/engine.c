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
#include "shader.h"
#include "manager.h"

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
    allocateMemories();
    createBuffers();
    createSwapchain();
    createFramebufferSet();
    loadShaders();
    createModules();
    createDescriptors();
    initializeAssets();
    loadAssets();
}

void loop() {
    draw(NULL);
}

void quit() {
    freeAssets();
    destroyModules();
    destroyDescriptors();
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
