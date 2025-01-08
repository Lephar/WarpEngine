#include "control.h"

// TODO: Not necessary here, remove when done
#include "helper.h"

#include "content.h"

extern SDL_Window *window;
extern VkExtent2D extent;

struct timespec timeCurrent;
float timeDelta; // In microseconds

vec2 mouseDelta;

vec3 position;
vec3 forward;
vec3 left;
vec3 up;
vec3 upGlobal;

vec3 movementInput;
vec3 movement;

mat4 viewMatrix;
mat4 projMatrix;

extern VkDeviceSize uniformBufferSize;
extern Uniform *uniformBuffer;
extern void *mappedSharedMemory;

void initializeControls() {
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    int32_t discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glm_vec2_zero(mouseDelta);

    glm_vec3_zero(position);
    glm_vec3_zero(forward);
    glm_vec3_zero(left);
    glm_vec3_zero(up);
    glm_vec3_zero(upGlobal);

    forward[1]  = 1.0f;
    left[0]     = 1.0f;
    up[2]       = 1.0f;
    upGlobal[2] = 1.0f;

    debug("Control variables created");
}

void generatePerspective() {
    float aspectRatio = (float) extent.width / (float) extent.height;
    float fieldOfView = M_PI_2;

    float nearPlane =   1.0f;
    float  farPlane = 100.0f;

    glm_perspective(fieldOfView, aspectRatio, nearPlane, farPlane, projMatrix);
    debug("Perspective created");
}

void processEvents() {
    struct timespec timePrevious = timeCurrent;
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);
    timeDelta = SEC_TO_MSEC * MSEC_TO_USEC * (timeCurrent.tv_sec - timePrevious.tv_sec) + (timeCurrent.tv_nsec - timePrevious.tv_nsec) / USEC_TO_NSEC;

    int32_t mouseX;
    int32_t mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);
    SDL_WarpMouseInWindow(window, extent.width / 2, extent.height / 2);

    mouseDelta[0] =  2.0f * mouseX / extent.width;
    mouseDelta[1] = -2.0f * mouseY / extent.height;

    // TODO: Limit vertical rotation on global up and down
    if(compareFloat(glm_vec2_norm2(mouseDelta), 0)) {
        glm_vec3_rotate(forward, mouseDelta[0], upGlobal);
        glm_vec3_rotate(left   , mouseDelta[0], upGlobal);
        glm_vec3_rotate(up     , mouseDelta[0], upGlobal);
        glm_vec3_rotate(forward, mouseDelta[1], left    );
        glm_vec3_rotate(up     , mouseDelta[1], left    );
    }

    int keyCount = 0;
    const uint8_t *states = SDL_GetKeyboardState(&keyCount);

    movementInput[0] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];
    movementInput[1] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    movementInput[2] = states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F];

    if(compareFloat(glm_vec3_norm2(movementInput), 0)) {
        vec3 forwardMovement;
        vec3 leftMovement;
        vec3 upMovement;

        glm_vec3_scale_as(movementInput, timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), movementInput);

        glm_vec3_scale_as(forward, movementInput[0], forwardMovement);
        glm_vec3_scale_as(left   , movementInput[1], leftMovement   );
        glm_vec3_scale_as(up     , movementInput[2], upMovement     );

        glm_vec3_add(forwardMovement, position, position);
        glm_vec3_add(leftMovement   , position, position);
        glm_vec3_add(upMovement     , position, position);
    }

    vec3 target;
    glm_vec3_add(position, forward, target);
    glm_lookat(position, target, upGlobal, viewMatrix);
    glm_mat4_mul(projMatrix, viewMatrix, *uniformBuffer);
}
