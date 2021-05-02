#include "renderSystem.h"
#include "renderSystemCore.h"
#include "renderSystemDevice.h"

zrCore core;
zrDevice device;

void initializeRenderer(void) {
    core = createCore();
    device = createDevice(core);
}

void destroyRenderer(void) {
    destroyDevice(device);
    destroyCore(core);
}