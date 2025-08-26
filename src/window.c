#include "window.h"

#include "file.h"
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
vec3 mainMovementInput;
vec3 freeMovementInput;

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
    // NOTICE: RenderDoc doesn't support Wayland yet
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
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
    mouseDelta[1] =  2.0f * mouseY / extent.height;

    int keyCount = 0;
    const bool *states = SDL_GetKeyboardState(&keyCount);

    freeMovementInput[0] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    freeMovementInput[1] = states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F];
    freeMovementInput[2] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];

    mainMovementInput[0] = states[SDL_SCANCODE_LEFT]    - states[SDL_SCANCODE_RIGHT];
    mainMovementInput[1] = states[SDL_SCANCODE_KP_PLUS] - states[SDL_SCANCODE_KP_MINUS];
    mainMovementInput[2] = states[SDL_SCANCODE_UP]      - states[SDL_SCANCODE_DOWN];

    if(compareFloat(glmc_vec3_norm2(freeMovementInput), 0.0f)) {
        glmc_vec3_scale_as(freeMovementInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), freeMovementInput);
    }

    if(compareFloat(glmc_vec3_norm2(mainMovementInput), 0.0f)) {
        glmc_vec3_scale_as(mainMovementInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), mainMovementInput);
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
