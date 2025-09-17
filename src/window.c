#include "window.h"

#include "numerics.h"
#include "logger.h"

SDL_Window *window;

int32_t windowWidth;
int32_t windowHeight;

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
    SDL_Vulkan_LoadLibrary(nullptr);

    debug("System initialized with %s driver", SDL_GetCurrentVideoDriver());
}

void *getSystemFunctionLoader() {
    return SDL_Vulkan_GetVkGetInstanceProcAddr();
}

const char *const *getRequiredExtensions(uint32_t requiredExtensionCount) {
    return SDL_Vulkan_GetInstanceExtensions(&requiredExtensionCount);
}

void createWindow(const char *title, int32_t width, int32_t height) {
    window = SDL_CreateWindow(title, width, height, SDL_WINDOW_VULKAN);
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

    debug("Window created: %dx%d", windowWidth, windowHeight);
}

void initializeMainLoop() {
#if DEBUG
    timer = SDL_AddTimer(SEC_TO_MSEC, timerCallback, nullptr);
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
            debug("Surface size changed, recreating swapchain");
            SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);
            resizeEvent = true;
        }
    }

    struct timespec timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (float) (timeCurrent.tv_sec - timePrevious.tv_sec) + (float) (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;

    float mouseX;
    float mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    SDL_WarpMouseInWindow(window, (float) windowWidth / 2.0f, (float) windowHeight / 2.0f);

    mouseDelta[0] = -2.0f * mouseX / (float) windowWidth;
    mouseDelta[1] =  2.0f * mouseY / (float) windowHeight;

    int keyCount = 0;
    const bool *states = SDL_GetKeyboardState(&keyCount);

    freeMovementInput[0] = (float) (states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D]);
    freeMovementInput[1] = (float) (states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F]);
    freeMovementInput[2] = (float) (states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S]);

    mainMovementInput[0] = (float) (states[SDL_SCANCODE_LEFT]    - states[SDL_SCANCODE_RIGHT]);
    mainMovementInput[1] = (float) (states[SDL_SCANCODE_KP_PLUS] - states[SDL_SCANCODE_KP_MINUS]);
    mainMovementInput[2] = (float) (states[SDL_SCANCODE_UP]      - states[SDL_SCANCODE_DOWN]);

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

    debug("System quit");
}
