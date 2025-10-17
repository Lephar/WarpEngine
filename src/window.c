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
float timeDelta; // In microseconds

vec2 mouseDelta;
vec3 joystickDelta;
vec3 secondaryKeyboardInput;
vec3 primaryKeyboardInput;

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

    //SDL_SetWindowTitle(window, title);
    debug("%s\tHas joystick: %u", title, SDL_HasJoystick());

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

        joystick = SDL_OpenJoystick(joysticks[0]);
        SDL_free(joysticks);
    }

    SDL_HideCursor();
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_SetWindowResizable(window, true);

    float discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glmc_vec2_zero(mouseDelta);

    glmc_vec3_zero(joystickDelta);

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
    mouseDelta[1] = -2.0f * mouseY / (float) windowHeight;

    if(joystick != nullptr) {
        const float joystickCoefficient = 0.03125f;

        mouseDelta[0] = joystickCoefficient * (float) SDL_GetJoystickAxis(joystick, 0) / (float) SHRT_MAX;
        mouseDelta[1] = joystickCoefficient * (float) SDL_GetJoystickAxis(joystick, 1) / (float) SHRT_MAX;

        primaryKeyboardInput[2] = timeDelta * (float) SDL_GetJoystickAxis(joystick, 2) / (float) SHRT_MAX / (SEC_TO_MSEC * MSEC_TO_USEC);

        const float joystickEpsilon = 0.000976562f;

        if(fabsf(mouseDelta[0]) < joystickEpsilon) {
            mouseDelta[0] = 0.0f;
        }

        if(fabsf(mouseDelta[1]) < joystickEpsilon) {
            mouseDelta[1] = 0.0f;
        }

        if(fabsf(primaryKeyboardInput[2]) < joystickEpsilon * timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC)) {
            primaryKeyboardInput[2] = 0.0f;
        }
    }
    /*
    int keyCount = 0;
    const bool *states = SDL_GetKeyboardState(&keyCount);

    primaryKeyboardInput[0] = (float) (states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D]);
    primaryKeyboardInput[1] = (float) (states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F]);
    primaryKeyboardInput[2] = (float) (states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S]);

    secondaryKeyboardInput[0] = (float) (states[SDL_SCANCODE_LEFT]    - states[SDL_SCANCODE_RIGHT]);
    secondaryKeyboardInput[1] = (float) (states[SDL_SCANCODE_KP_PLUS] - states[SDL_SCANCODE_KP_MINUS]);
    secondaryKeyboardInput[2] = (float) (states[SDL_SCANCODE_UP]      - states[SDL_SCANCODE_DOWN]);

    if(compareFloat(glmc_vec3_norm2(primaryKeyboardInput), 0.0f)) {
        glmc_vec3_scale_as(primaryKeyboardInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), primaryKeyboardInput);
    }

    if(compareFloat(glmc_vec3_norm2(secondaryKeyboardInput), 0.0f)) {
        glmc_vec3_scale_as(secondaryKeyboardInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), secondaryKeyboardInput);
    }
    */
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
