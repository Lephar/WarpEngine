#include "control.h"

#include "window.h"
#include "content.h"

#include "numerics.h"
#include "logger.h"

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

float moveSpeed;

void initializeControls() {
    clock_gettime(CLOCK_MONOTONIC, &timeCurrent);

    int32_t discard;
    SDL_GetRelativeMouseState(&discard, &discard);
    glmc_vec2_zero(mouseDelta);

    glmc_vec3_zero(position);
    glmc_vec3_zero(forward);
    glmc_vec3_zero(left);
    glmc_vec3_zero(up);
    glmc_vec3_zero(upGlobal);

    position[2] = 8.0f; // TODO: Load starting position as an asset
    forward[1]  = 1.0f;
    left[0]     = 1.0f;
    up[2]       = 1.0f;
    upGlobal[2] = 1.0f;

    moveSpeed = 10.0f;

    debug("Control variables created");
}

void generatePerspective() {
    float aspectRatio = (float) extent.width / (float) extent.height;
    float fieldOfView = M_PI_4;

    float nearPlane = 1.0f;
    float  farPlane = 1000.0f;

    // TODO: Make a separate light unit
    glmc_vec3_one(uniformBuffer->ambientLight);

    glmc_perspective_lh_zo(fieldOfView, aspectRatio, nearPlane, farPlane, uniformBuffer->proj);
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
    if(compareFloat(glmc_vec2_norm2(mouseDelta), 0)) {
        glmc_vec3_rotate(forward, mouseDelta[0], upGlobal);
        glmc_vec3_rotate(left   , mouseDelta[0], upGlobal);
        glmc_vec3_rotate(up     , mouseDelta[0], upGlobal);
        glmc_vec3_rotate(forward, mouseDelta[1], left    );
        glmc_vec3_rotate(up     , mouseDelta[1], left    );
    }

    int keyCount = 0;
    const uint8_t *states = SDL_GetKeyboardState(&keyCount);

    movementInput[0] = states[SDL_SCANCODE_W] - states[SDL_SCANCODE_S];
    movementInput[1] = states[SDL_SCANCODE_A] - states[SDL_SCANCODE_D];
    movementInput[2] = states[SDL_SCANCODE_R] - states[SDL_SCANCODE_F];

    if(compareFloat(glmc_vec3_norm2(movementInput), 0)) {
        vec3 forwardMovement;
        vec3 leftMovement;
        vec3 upMovement;

        glmc_vec3_scale_as(movementInput, moveSpeed * timeDelta / (SEC_TO_MSEC * MSEC_TO_USEC), movementInput);

        glmc_vec3_scale_as(forward, movementInput[0], forwardMovement);
        glmc_vec3_scale_as(left   , movementInput[1], leftMovement   );
        glmc_vec3_scale_as(up     , movementInput[2], upMovement     );

        glmc_vec3_add(forwardMovement, position, position);
        glmc_vec3_add(leftMovement   , position, position);
        glmc_vec3_add(upMovement     , position, position);
    }

    vec3 target;
    glmc_vec3_add(position, forward, target);

    glmc_lookat_lh_zo(GLM_VEC3_ZERO, forward, upGlobal, uniformBuffer->skyboxView);
    glmc_mat4_mul(uniformBuffer->proj, uniformBuffer->skyboxView, uniformBuffer->skyboxCamera);

    glmc_lookat_lh_zo(position, target, upGlobal, uniformBuffer->view);
    glmc_mat4_mul(uniformBuffer->proj, uniformBuffer->view, uniformBuffer->camera);
}
