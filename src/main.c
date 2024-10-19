#include "window.h"
#include "instance.h"
#include "device.h"
#include "queue.h"
#include "surface.h"
#include "memory.h"
#include "swapchain.h"
#include "framebuffer.h"

int main(int argc, char *argv[]) {
    (void)argc;

    initialize();
    createWindow(argv[0], 800, 600);
    createInstance();
    selectPhysicalDevice();
    generateQueueDetails();
    createDevice();
    getQueues();
    createSurface();
    createSwapchain();
    allocateMemories();
    createFramebuffers();

    draw(NULL);

    destroyFramebuffers();
    freeMemories();
    destroySwapchain();
    destroySurface();
    clearQueues();
    destroyDevice();
    destroyInstance();
    destroyWindow();
    quit();
}
