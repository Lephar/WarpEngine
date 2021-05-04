#include "renderSystem.h"
#include "renderSystemCore.h"
#include "renderSystemDevice.h"

Core core;
Device device;

void initializeRenderer(void) {
    core = createCore();
    device = createDevice(core);
}

void destroyRenderer(void) {
    destroyDevice(device);
    destroyCore(core);
}