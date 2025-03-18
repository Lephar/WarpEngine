#include "window.h"

#include "surface.h"
#include "config.h"

#include "helper.h"
#include "control.h"

SDL_Window *window = NULL;

uint32_t frameIndex = 0;
uint32_t frameIndexCheckpoint = 0;

Status status;

#if DEBUG
SDL_TimerID timer = 0;

uint32_t timerCallback(uint32_t interval, void *userData) {
    (void) userData;

    uint32_t frameDifference = frameIndex - frameIndexCheckpoint;
    frameIndexCheckpoint = frameIndex;

    char title[INT8_MAX];
    sprintf(title, "Frame: %u\tFPS: %u", frameIndex, frameDifference);

    SDL_SetWindowTitle(window, title);

    return interval;
}
#endif

void initializeSystem() {
    // TODO: Ubuntu has issues with Wayland and RenderDoc doesn't support it yet, wait for update
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11,wayland,windows");
    SDL_SetHint(SDL_HINT_AUDIODRIVER, "pulseaudio,pipewire,directsound");
    SDL_Init(SDL_INIT_EVERYTHING);

    debug("System initialized:");
    debug("\tVideo Driver: %s", SDL_GetCurrentVideoDriver());
    debug("\tAudio Driver: %s", SDL_GetCurrentAudioDriver());
}

PFN_vkVoidFunction getSystemLoader() {
    return SDL_Vulkan_GetVkGetInstanceProcAddr();
}

void createWindow() {
    window = SDL_CreateWindow(executableName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, extent.width, extent.height, SDL_WINDOW_VULKAN);

    SDL_Vulkan_GetDrawableSize(window, (int32_t *) &extent.width, (int32_t *) &extent.height);

    debug("Window created:");
    debug("\tWidth:  %u", extent.width );
    debug("\tHeight: %u", extent.height);
}

SDL_bool getWindowExtensions(uint32_t *extensionCount, const char **extensionNames) {
    return SDL_Vulkan_GetInstanceExtensions(window, extensionCount, extensionNames);
}

void initializeMainLoop() {
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowResizable(window, SDL_TRUE);

#if DEBUG
    timer = SDL_AddTimer(SEC_TO_MSEC, timerCallback, NULL);
#endif

    initializeControls();
    generatePerspective();

    debug("Main loop initialized");
}

void pollEvents() {
    frameIndex++;

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            status.quit = SDL_TRUE;
            return;
        } else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            debug("Recreating swapchain");
            SDL_Vulkan_GetDrawableSize(window, (int32_t *) &extent.width, (int32_t *) &extent.height);
            generatePerspective();
            status.resize = SDL_TRUE;
        }
    }

    processEvents();
}

void finalizeMainLoop() {
#if DEBUG
    SDL_RemoveTimer(timer);
#endif

    debug("Main loop finalized");
}

void destroyWindow() {
    SDL_DestroyWindow(window);

    debug("Window destroyed");
}

void quitSystem() {
    SDL_Quit();

    debug("System quitted");
}
