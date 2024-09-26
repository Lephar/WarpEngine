#include "window.h"
#include "instance.h"
#include "surface.h"
#include "device.h"
#include "queue.h"

int main(int argc, char *argv[]) {
    (void)argc;

    initialize();
    createWindow(argv[0], 800, 600);
    createInstance();
    createSurface();
    selectPhysicalDevice();
    generateQueueDetails();
    createDevice();
    retrieveQueues();

    draw(NULL);

    destroyDevice();
    destroySurface();
    destroyInstance();
    destroyWindow();
    quit();
}
