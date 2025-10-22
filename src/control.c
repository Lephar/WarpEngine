#include "control.h"

#include "window.h"
#include "asset.h"

vec4 worldLeft;
vec4 worldUp;
vec4 worldForward;

uint32_t controlSetCount;
PControlSet controlSets;

// TODO: Are we sure that these directions are correct?
void initializeWorld() {
    worldLeft[0] = 1.0f;
    worldLeft[1] = 0.0f;
    worldLeft[2] = 0.0f;
    worldLeft[3] = 0.0f;

    worldUp[0] = 0.0f;
    worldUp[1] = 1.0f;
    worldUp[2] = 0.0f;
    worldUp[3] = 0.0f;

    worldForward[0] = 0.0f;
    worldForward[1] = 0.0f;
    worldForward[2] = 1.0f;
    worldForward[3] = 0.0f;
}

uint32_t initializeControlSet(float turnSpeed, float moveSpeed, void (*controlFunction)(PControlSet)) {
    assert(controlSetCount < nodeCountLimit);
    const uint32_t controlSetIndex = controlSetCount;
    controlSetCount++;

    PControlSet controlSet = &controlSets[controlSetIndex];

    controlSet->turnSpeed = turnSpeed;
    controlSet->moveSpeed = moveSpeed;

    glmc_mat4_identity(controlSet->transform);

    controlSet->controlFunction = controlFunction;

    return controlSetIndex;
}

void bindControlSet(uint32_t controlSetIndex, uint32_t nodeIndex) {
    PControlSet controlSet = &controlSets[controlSetIndex];
    PNode node = &nodes[nodeIndex];

    node->controlSet = controlSet;
}

void updateControlSet(PControlSet controlSet) {
    if(controlSet->controlFunction) {
        controlSet->controlFunction(controlSet);
    }
}

// TODO: Free movement is a bit janky
void firstPersonControl(PControlSet controlSet) {
    vec2 mouseMovement;
    glmc_vec2_scale(mouseDelta, controlSet->turnSpeed, mouseMovement);

    // TODO: Limit vertical rotation on global up and down
    glmc_rotate(controlSet->transform, mouseMovement[0], worldUp);
    glmc_rotate(controlSet->transform, mouseMovement[1], worldLeft);

    vec3 keyboardMovement;
    glmc_vec3_scale(primaryKeyboardInput, controlSet->moveSpeed, keyboardMovement);

    keyboardMovement[0] *= -1;
    keyboardMovement[2] *= -1;

    glmc_translate(controlSet->transform, keyboardMovement);
}

void fixedWingControl(PControlSet controlSet) {
    vec3 scaledJoystickRotation;
    glmc_vec3_scale(joystickRotation, controlSet->turnSpeed, scaledJoystickRotation);

    scaledJoystickRotation[0] *= -1;
    scaledJoystickRotation[2] *= -1;

    // TODO: Limit vertical rotation on global up and down
    glmc_rotate(controlSet->transform, scaledJoystickRotation[0], worldForward);
    glmc_rotate(controlSet->transform, scaledJoystickRotation[1], worldLeft);
    glmc_rotate(controlSet->transform, scaledJoystickRotation[2], worldUp);

    vec3 scaledJoystickMovement;
    glmc_vec3_scale(joystickMovement, controlSet->moveSpeed, scaledJoystickMovement);
    scaledJoystickMovement[2] -= 2 * timeDelta * controlSet->moveSpeed;

    glmc_translate(controlSet->transform, scaledJoystickMovement);
}
