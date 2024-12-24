#include "control.h"

// TODO: Not necessary here, remove when done
#include "helper.h"

#include "content.h"

struct timespec timeCurrent;
float timeDelta; // In microseconds

extern VkExtent2D extent;

vec2 mouseDelta;

vec3 position;
vec3 forward;
vec3 left;
vec3 upGlobal;

vec2 movementInput;
vec2 movement;

void resetControls() {
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    int32_t discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glm_vec2_zero(mouseDelta);
}

void processControlEvents() {
    struct timespec timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (timeCurrent.tv_sec - timePrevious.tv_sec) + (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;

    int32_t mouseX;
    int32_t mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    mouseDelta[0] = 2.0f * mouseX / extent.width;
    mouseDelta[1] = 2.0f * mouseY / extent.height;

    int keyCount = 0;
    const uint8_t *states = SDL_GetKeyboardState(&keyCount);

    movementInput[0] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    movementInput[1] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];
}

void postprocessFrameControls() {
}
