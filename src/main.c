#include "window.h"
#include "instance.h"
#include "surface.h"
#include "device.h"

int main(int argc, char *argv[]) {
    (void) argc;

    initialize();
    createWindow(argv[0], 800, 600);
    createInstance();
    createSurface();
    selectPhysicalDevice();
    createDevice();

    draw(NULL);

    destroyDevice();
    destroySurface();
    destroyInstance();
    destroyWindow();
    quit();
}