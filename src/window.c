#include "window.h"

#include "config.h"
#include "numerics.h"
#include "logger.h"

PFN_vkGetInstanceProcAddr systemFunctionLoader;

uint32_t systemExtensionCount;
char const *const *systemExtensions;

SDL_Window *window;
VkExtent2D extent;

uint32_t frameIndex;
uint32_t frameIndexCheckpoint;

struct timespec timeCurrent;
float timeDelta; // In microseconds

vec2 mouseDelta;
vec3 movementInput;

bool resizeEvent;
bool quitEvent;

#if DEBUG
SDL_TimerID timer;

uint32_t timerCallback(void *userData, uint32_t id, uint32_t interval) {
    (void) userData;
    (void) id;

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
    //SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Vulkan_LoadLibrary(NULL);

    systemFunctionLoader = (PFN_vkGetInstanceProcAddr) SDL_Vulkan_GetVkGetInstanceProcAddr();
    systemExtensions = SDL_Vulkan_GetInstanceExtensions(&systemExtensionCount);

    debug("System initialized with %s driver", SDL_GetCurrentVideoDriver());
}

void *loadSystemFunction(const char *name) {
    void  *systemFunction = systemFunctionLoader(NULL, name);
    assert(systemFunction);
    return systemFunction;
}

void createWindow() {
    window = SDL_CreateWindow(executableName, extent.width, extent.height, SDL_WINDOW_VULKAN);

    SDL_GetWindowSizeInPixels(window, (int32_t *) &extent.width, (int32_t *) &extent.height);

    debug("Window created:");
    debug("\tWidth:  %u", extent.width );
    debug("\tHeight: %u", extent.height);
}

void initializeMainLoop() {
#if DEBUG
    timer = SDL_AddTimer(SEC_TO_MSEC, timerCallback, NULL);
#endif

    SDL_HideCursor();
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_SetWindowResizable(window, true);

    float discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glmc_vec2_zero(mouseDelta);

    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    debug("Main loop initialized");
}

void pollEvents() {
    frameIndex++;

    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)) {
            quitEvent = true;
            return;
        } else if(event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
            debug("Recreating swapchain");
            SDL_GetWindowSizeInPixels(window, (int32_t *) &extent.width, (int32_t *) &extent.height);
            resizeEvent = true;
        }
    }

    struct timespec timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (timeCurrent.tv_sec - timePrevious.tv_sec) + (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;

    float mouseX;
    float mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    SDL_WarpMouseInWindow(window, extent.width / 2.0f, extent.height / 2.0f);

    mouseDelta[0] = -2.0f * mouseX / extent.width;
    mouseDelta[1] = -2.0f * mouseY / extent.height;

    int keyCount = 0;
    const bool *states = SDL_GetKeyboardState(&keyCount);

    movementInput[0] = states[SDL_SCANCODE_D] - states[SDL_SCANCODE_A];
    movementInput[1] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];
    movementInput[2] = states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F];

    if(compareFloat(glmc_vec3_norm2(movementInput), 0.0f)) {
        glmc_vec3_scale_as(movementInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), movementInput);
    }
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
