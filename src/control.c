#include "control.h"

// TODO: Not necessary here, remove when done
#include "helper.h"

#define  SEC_TO_MSEC 1000.0f
#define MSEC_TO_USEC 1000.0f
#define USEC_TO_NSEC 1000.0f

struct timespec timePrevious;
struct timespec timeCurrent;
float timeDelta; // In microseconds

extern VkExtent2D extent;

vec2 mousePositionPrevious;
vec2 mousePosition;

vec3 position;
vec3 forward;
vec3 left;
vec3 upGlobal;

vec2 movementInput;
vec2 movement;

void resetControls() {
    clock_gettime(CLOCK_MONOTONIC, &timePrevious);
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    glm_vec2_zero(mousePositionPrevious);
    glm_vec2_zero(mousePosition);
}

void preprocessFrameControls() {
    timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (timeCurrent.tv_sec - timePrevious.tv_sec) + (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;
    //debug("%g", timeDelta);

    int keyCount = 0;
    const uint8_t *states = SDL_GetKeyboardState(&keyCount);
    debug("W:%u A:%u S:%u D:%u", states[SDL_SCANCODE_W], states[SDL_SCANCODE_A], states[SDL_SCANCODE_S], states[SDL_SCANCODE_D]);

    movementInput[0] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    movementInput[1] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];

    //glm_vec2_zero(movement);
    glm_vec2_copy(mousePosition, mousePositionPrevious);
}

void mouseMove(SDL_MouseMotionEvent event) {
    mousePosition[0] = 2.0f * event.x / extent.width  - 1.0f;
    mousePosition[1] = 2.0f * event.y / extent.height - 1.0f;
}

void keyDown(SDL_KeyboardEvent event) {
    (void) event;
}

void keyUp(SDL_KeyboardEvent event) {
    (void) event;
}

void postprocessFrameControls() {
    //glm_vec2_norm(movement);

    debug("Mouse: %g %g -> %g %g", mousePositionPrevious[0], mousePositionPrevious[1], mousePosition[0], mousePosition[1]);
    debug("Movement: %g %g", movementInput[0], movementInput[1]);
}
