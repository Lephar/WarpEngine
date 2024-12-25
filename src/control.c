#include "control.h"

// TODO: Not necessary here, remove when done
#include "helper.h"

#include "content.h"

extern SDL_Window *window;
extern VkExtent2D extent;

struct timespec timeCurrent;
float timeDelta; // In microseconds

float aspectRatio;
float fieldOfView;

float nearPlane;
float  farPlane;

vec2 mouseDelta;

vec3 position;
vec3 forward;
vec3 left;
vec3 upGlobal;

vec2 movementInput;
vec2 movement;

mat4 viewMatrix;
mat4 projMatrix;

extern VkDeviceSize uniformBufferSize;
extern Uniform *uniformBuffer;
extern void *mappedSharedMemory;

void resetControls() {
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    int32_t discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glm_vec2_zero(mouseDelta);

    aspectRatio = (float) extent.width / (float) extent.height;
    fieldOfView = M_PI_2;

    nearPlane =  0.1f;
     farPlane = 10.0f;

    glm_vec3_zero(position);
    glm_vec3_zero(forward);
    glm_vec3_zero(left);
    glm_vec3_zero(upGlobal);

    forward[2]  = 1.0f;
    left[0]     = 1.0f;
    upGlobal[1] = 1.0f;

    glm_perspective(fieldOfView, aspectRatio, nearPlane, farPlane, projMatrix);
}

void processControlEvents() {
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

    movementInput[0] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    movementInput[1] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];

    if(compareFloat(glm_vec2_norm2(mouseDelta), 0)) {
        glm_vec3_rotate(left, mouseDelta[0], upGlobal);
        glm_vec3_rotate(forward, mouseDelta[0], upGlobal);
        // TODO: Limit vertical rotation on global up and down
        glm_vec3_rotate(forward, mouseDelta[1], left);
    }

    if(compareFloat(glm_vec2_norm2(movementInput), 0)) {
        glm_vec2_scale_as(movementInput, timeDelta, movementInput);
    }

    vec3 target;
    glm_vec3_add(position, forward, target);
    glm_lookat(position, target, upGlobal, viewMatrix);
    glm_mat4_mul(projMatrix, viewMatrix, uniformBuffer->transform);
}
