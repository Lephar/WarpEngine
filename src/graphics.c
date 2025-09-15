#include "graphics.h"

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

#include "thread.h"

void startGraphicsModule() {
    initializeSystem();
    createWindow();

    //PThread systemThread = dispatchThread(0, nullptr, initializeSystem);
    //PThread windowThread = dispatchThread(1, &systemThread, createWindow);

    //PThread instanceThread = dispatchThread(1, &systemThread, createInstance);
    PThread instanceThread = dispatchThread(0, nullptr, createInstance);
    PThread physicalDeviceThread = dispatchThread(1, &instanceThread, selectPhysicalDevice);
    PThread queueDetailsThread = dispatchThread(1, &physicalDeviceThread, generateQueueDetails);

    PThread surfaceDependencies[] = {
        //windowThread,
        queueDetailsThread
    };

    uint32_t surfaceDependencyCount = sizeof(surfaceDependencies) / sizeof(PThread);

    PThread surfaceThread = dispatchThread(surfaceDependencyCount, surfaceDependencies, createSurface);

    PThread deviceThread = dispatchThread(1, &queueDetailsThread, createDevice);
    PThread queueThread = dispatchThread(1, &deviceThread, getQueues);

    PThread pipelineThread = dispatchThread(1, &deviceThread, createPipeline);
    PThread moduleThread = dispatchThread(1, &pipelineThread, createModules);

    PThread memoryThread = dispatchThread(1, &deviceThread, allocateMemories);
    PThread bufferThread = dispatchThread(1, &memoryThread, createBuffers);

    PThread contentBuffersDependencies[] = {
        pipelineThread,
        bufferThread
    };

    uint32_t contentBuffersDependencyCount = sizeof(contentBuffersDependencies) / sizeof(PThread);

    PThread contentBuffersThread = dispatchThread(contentBuffersDependencyCount, contentBuffersDependencies, createContentBuffers);

    PThread contentDependencies[] = {
        queueThread,
        contentBuffersThread
    };

    uint32_t contentDependencyCount = sizeof(contentDependencies) / sizeof(PThread);

    PThread contentThread = dispatchThread(contentDependencyCount, contentDependencies, loadContent);

    PThread swapchainDependencies[] = {
        surfaceThread,
        queueThread,
        contentThread
    };

    uint32_t swapchainDependencyCount = sizeof(swapchainDependencies) / sizeof(PThread);

    PThread swapchainThread = dispatchThread(swapchainDependencyCount, swapchainDependencies, createSwapchain);
    PThread framebufferSetThread = dispatchThread(1, &swapchainThread, createFramebufferSet);

    waitThread(moduleThread);
    waitThread(framebufferSetThread);
}

void initEngine() {
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

    resizeEvent = false;
}

void loopEngine() {
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

void quitEngine() {
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
