#include "control.h"

// TODO: Not necessary here, remove when done
#include "helper.h"

extern VkExtent2D extent;

vec2 mousePositionPrevious;
vec2 mousePosition;

vec3 position;
vec3 forward;
vec3 left;
vec3 globalUp;

vec2 movementInput;
vec2 movement;

void resetControls() {
    glm_vec2_zero(mousePositionPrevious);
    glm_vec2_zero(mousePosition);
}

void preprocessFrameControls() {
    //glm_vec2_zero(movement);
    glm_vec2_copy(mousePosition, mousePositionPrevious);
}

void mouseMove(SDL_MouseMotionEvent event) {
    mousePosition[0] = 2.0f * event.x / extent.width  - 1.0f;
    mousePosition[1] = 2.0f * event.y / extent.height - 1.0f;
}

void keyDown(SDL_KeyboardEvent event) {
    movementInput[0] += (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_A);
    movementInput[0] -= (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_D);
    movementInput[1] += (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_W);
    movementInput[1] -= (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_S);
}

void keyUp(SDL_KeyboardEvent event) {
    movementInput[0] -= (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_A);
    movementInput[0] += (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_D);
    movementInput[1] -= (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_W);
    movementInput[1] += (event.repeat == 0) * (event.keysym.scancode == SDL_SCANCODE_S);
}

void postprocessFrameControls() {
    //glm_vec2_norm(movement);

    debug("Mouse: %g %g -> %g %g", mousePositionPrevious[0], mousePositionPrevious[1], mousePosition[0], mousePosition[1]);
    debug("Movement: %g %g", movementInput[0], movementInput[1]);
}
