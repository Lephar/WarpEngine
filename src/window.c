#include "window.h"

#include "helper.h"
#include "control.h"

extern char executableName[];

SDL_bool systemInitialized = SDL_FALSE;
SDL_bool windowCreated = SDL_FALSE;
extern SDL_bool surfaceCreated;

PFN_vkGetInstanceProcAddr getInstanceProcAddr;
SDL_Window *window = NULL;
VkExtent2D extent = {};
uint32_t requiredInstanceExtensionCount = 0;
const char **requiredInstanceExtensionNames = NULL;

extern VkInstance instance;
extern VkSurfaceKHR surface;

SDL_TimerID timer = 0;

uint32_t frameIndex = 0;
uint32_t frameIndexCheckpoint = 0;

void initializeSystem() {
    assert(!systemInitialized);

// TODO: Temporarily use X11 for debug builds because RenderDoc doesn't support Wayland
#ifndef NDEBUG
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11,wayland,windows");
    SDL_SetHint(SDL_HINT_AUDIODRIVER, "pulseaudio,pipewire,directsound");
#else
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11,windows");
    SDL_SetHint(SDL_HINT_AUDIODRIVER, "pipewire,pulseaudio,directsound");
#endif //NDEBUG

    SDL_Init(SDL_INIT_EVERYTHING);
    debug("SDL Video Driver: %s", SDL_GetCurrentVideoDriver());
    debug("SDL Audio Driver: %s", SDL_GetCurrentAudioDriver());

    SDL_Vulkan_LoadLibrary(NULL);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    getInstanceProcAddr = SDL_Vulkan_GetVkGetInstanceProcAddr();

    systemInitialized = SDL_TRUE;
    debug("System initialized");
}

void createWindow() {
    assert(systemInitialized && !windowCreated);

    window = SDL_CreateWindow(executableName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, extent.width, extent.height, SDL_WINDOW_VULKAN);
    SDL_Vulkan_GetDrawableSize(window, (int32_t *)&extent.width, (int32_t *)&extent.height);

    SDL_Vulkan_GetInstanceExtensions(window, &requiredInstanceExtensionCount, NULL);

    requiredInstanceExtensionNames = malloc(requiredInstanceExtensionCount * sizeof(const char *));
    SDL_Vulkan_GetInstanceExtensions(window, &requiredInstanceExtensionCount, requiredInstanceExtensionNames);

    windowCreated = SDL_TRUE;
    debug("Window created:");
    debug("\tWidth:  %u", extent.width );
    debug("\tHeight: %u", extent.height);
}

#ifndef NDEBUG
uint32_t timerCallback(uint32_t interval, void *userData) {
    (void) userData;

    uint32_t frameDifference = frameIndex - frameIndexCheckpoint;
    frameIndexCheckpoint = frameIndex;

    char title[INT8_MAX];
    sprintf(title, "Frame: %u\tFPS: %u", frameIndex, frameDifference);

    SDL_SetWindowTitle(window, title);

    return interval;
}
#endif //NDEBUG

void initializeMainLoop() {
    resetControls();

#ifndef NDEBUG
    timerCallback(0, NULL); // Call it immediatelly once

    timer = SDL_AddTimer(1000, timerCallback, NULL);
    assert(timer);
#endif //NDEBUG

    debug("Main loop initialized");
}

SDL_bool pollEvents() {
    frameIndex++;

    preprocessFrameControls();

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            return SDL_FALSE;
        }
    }

    postprocessFrameControls();

    return SDL_TRUE;
}

void finalizeMainLoop() {
#ifndef NDEBUG
    SDL_RemoveTimer(timer);
#endif //NDEBUG

    debug("Main loop finalized");
}

void destroyWindow() {
    assert(!surfaceCreated && windowCreated);

    free(requiredInstanceExtensionNames);
    requiredInstanceExtensionCount = 0;

    SDL_DestroyWindow(window);
    window = NULL;

    extent.height = 0;
    extent.width = 0;

    windowCreated = SDL_FALSE;
    debug("Window destroyed");
}

void quitSystem() {
    assert(!windowCreated && systemInitialized);

    getInstanceProcAddr = NULL;
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    systemInitialized = SDL_FALSE;
    debug("System quitted");
}
