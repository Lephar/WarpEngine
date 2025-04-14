#include "window.h"

#include "config.h"
#include "control.h"
#include "numerics.h"
#include "logger.h"

PFN_vkGetInstanceProcAddr systemLoader;

SDL_Window *window;
VkExtent2D extent;

uint32_t frameIndex;
uint32_t frameIndexCheckpoint;

struct timespec timeCurrent;
float timeDelta; // In microseconds

vec2 mouseDelta;
vec3 movementInput;

SDL_bool resizeEvent;
SDL_bool quitEvent;

#if DEBUG
SDL_TimerID timer;

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

    SDL_Vulkan_LoadLibrary(NULL);
    systemLoader = SDL_Vulkan_GetVkGetInstanceProcAddr();

    debug("System initialized:");
    debug("\tVideo Driver: %s", SDL_GetCurrentVideoDriver());
    debug("\tAudio Driver: %s", SDL_GetCurrentAudioDriver());
}

void *loadSystemFunction(const char *name) {
    void  *systemFunction = systemLoader(NULL, name);
    assert(systemFunction);
    return systemFunction;
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
#if DEBUG
    timer = SDL_AddTimer(SEC_TO_MSEC, timerCallback, NULL);
#endif

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetWindowResizable(window, SDL_TRUE);

    int32_t discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glmc_vec2_zero(mouseDelta);

    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    generatePerspective();

    debug("Main loop initialized");
}

void pollEvents() {
    frameIndex++;

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            quitEvent = SDL_TRUE;
            return;
        } else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            debug("Recreating swapchain");
            SDL_Vulkan_GetDrawableSize(window, (int32_t *) &extent.width, (int32_t *) &extent.height);
            generatePerspective();
            resizeEvent = SDL_TRUE;
        }
    }

    struct timespec timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (timeCurrent.tv_sec - timePrevious.tv_sec) + (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;

    int32_t mouseX;
    int32_t mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    SDL_WarpMouseInWindow(window, extent.width / 2, extent.height / 2);

    mouseDelta[0] = -2.0f * mouseX / extent.width;
    mouseDelta[1] = -2.0f * mouseY / extent.height;

    int keyCount = 0;
    const uint8_t *states = SDL_GetKeyboardState(&keyCount);

    movementInput[0] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];
    movementInput[1] = states[SDL_SCANCODE_D] - states[SDL_SCANCODE_A];
    movementInput[2] = states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F];

    glmc_vec3_scale_as(movementInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), movementInput);
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
