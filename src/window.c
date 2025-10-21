#include "window.h"

#include "system.h"
#include "numerics.h"
#include "logger.h"

SDL_Window *window;

int32_t windowWidth;
int32_t windowHeight;

uint32_t frameIndex;
uint32_t frameIndexCheckpoint;

struct timespec timeCurrent;
float timeDelta; // In seconds

vec2 mouseDelta;
vec3 primaryKeyboardInput;
vec3 secondaryKeyboardInput;
vec3 joystickRotation;
vec3 joystickMovement;

SDL_Joystick *joystick = nullptr;

bool resizeEvent;
bool quitEvent;

#if DEBUG
SDL_TimerID timer;

uint32_t timerCallback(void *userData, uint32_t id, uint32_t interval) {
    (void) userData;
    (void) id;

    uint32_t frameDifference = frameIndex - frameIndexCheckpoint;
    frameIndexCheckpoint = frameIndex;

    char title[UINT8_MAX];
    sprintf(title, "Frame: %u\tFPS: %u", frameIndex, frameDifference);

    //SDL_SetWindowTitle(window, title); // TODO: Causes segfault on Ubuntu
    debug("%s", title);

    return interval;
}
#endif

void createWindow() {
    window = SDL_CreateWindow(applicationName, windowWidth, windowHeight, SDL_WINDOW_VULKAN);
    SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

    debug("Window created: %dx%d", windowWidth, windowHeight);
}

void initializeMainLoop() {
#if DEBUG
    timer = SDL_AddTimer(SEC_TO_MSEC, timerCallback, nullptr);
#endif

    if(SDL_HasJoystick()) {
        int32_t joystickCount = 0;
        SDL_JoystickID *joysticks = SDL_GetJoysticks(&joystickCount);

        assert(joystickCount > 0);
        debug("%d joysticks found", joystickCount);

        joystick = SDL_OpenJoystick(*joysticks);
        SDL_free(joysticks);

        debug("Joystick connected", joystickCount);
    }

    SDL_HideCursor();
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_SetWindowResizable(window, true);

    float discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glmc_vec2_zero(mouseDelta);

    glmc_vec3_zero(joystickRotation);
    glmc_vec3_zero(joystickMovement);

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
    timeDelta = (float) (timeCurrent.tv_sec - timePrevious.tv_sec) + (float) (timeCurrent.tv_nsec - timePrevious.tv_nsec) / (SEC_TO_MSEC * MSEC_TO_USEC * USEC_TO_NSEC);

    float mouseX;
    float mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    SDL_WarpMouseInWindow(window, (float) windowWidth / 2.0f, (float) windowHeight / 2.0f);

    mouseDelta[0] = -2.0f * mouseX / (float) windowWidth;
    mouseDelta[1] = -2.0f * mouseY / (float) windowHeight;

    if(joystick != nullptr) {
        const int32_t joystickAxesCount = 3;
        assert(joystickAxesCount <= SDL_GetNumJoystickAxes(joystick));

        for(int32_t joystickAxisIndex = 0; joystickAxisIndex < joystickAxesCount; joystickAxisIndex++) {
            const int16_t joystickEpsilon = 1024;
            int16_t joystickAxisValue = SDL_GetJoystickAxis(joystick, joystickAxisIndex);
            joystickRotation[joystickAxisIndex] = abs(joystickAxisValue) < joystickEpsilon ? 0.0f : timeDelta * (float) joystickAxisValue / -SHRT_MIN;
            debug("\tAxis %u: %g", joystickAxisIndex, joystickRotation[joystickAxisIndex]);
        }
    }

    int keyCount = 0;
    const bool *states = SDL_GetKeyboardState(&keyCount);

    primaryKeyboardInput[0] = (float) (states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D]);
    primaryKeyboardInput[1] = (float) (states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F]);
    primaryKeyboardInput[2] = (float) (states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S]);

    secondaryKeyboardInput[0] = (float) (states[SDL_SCANCODE_LEFT]    - states[SDL_SCANCODE_RIGHT]);
    secondaryKeyboardInput[1] = (float) (states[SDL_SCANCODE_KP_PLUS] - states[SDL_SCANCODE_KP_MINUS]);
    secondaryKeyboardInput[2] = (float) (states[SDL_SCANCODE_UP]      - states[SDL_SCANCODE_DOWN]);

    if(compareFloat(glmc_vec3_norm2(primaryKeyboardInput), 0.0f)) {
        glmc_vec3_scale_as(primaryKeyboardInput, timeDelta, primaryKeyboardInput);
    }

    if(compareFloat(glmc_vec3_norm2(secondaryKeyboardInput), 0.0f)) {
        glmc_vec3_scale_as(secondaryKeyboardInput, timeDelta, secondaryKeyboardInput);
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
