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

char path[PATH_MAX];
char name[PATH_MAX];

extern VkExtent2D extent;

void initialize(int argc, char *argv[]) {
    debug("argc:     %d", argc);
    debug("PATH_MAX: %d", PATH_MAX);

    const char * separator = strrchr(argv[0], '/');

    if(separator == NULL) {
        getcwd(path, PATH_MAX);
        strncpy(name, argv[0], PATH_MAX);
    } else {
        strncpy(path, argv[0], separator - argv[0]);
        strncpy(name, separator + 1, PATH_MAX);
    }

    path[PATH_MAX - 1] = '\0';
    name[PATH_MAX - 1] = '\0';

    debug("Path: %s", path);
    debug("Name: %s", name);

    if(argc >= 3) {
        extent.width  = atoi(argv[1]);
        extent.height = atoi(argv[2]);
    } else {
        extent.width  = 800;
        extent.height = 600;
    }

    debug("Width:  %d", extent.width);
    debug("Height: %d", extent.height);

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
