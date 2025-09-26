#include "control.h"

#include "asset.h"
#include "logger.h"
#include "window.h"

vec4 worldLeft;
vec4 worldUp;
vec4 worldForward;

uint32_t controlSetCount;
PControlSet controlSets;

// TODO: All the functions below will be moved to separate unit
void initializeWorld() {
    worldLeft[0] = -1.0f;
    worldLeft[1] =  0.0f;
    worldLeft[2] =  0.0f;
    worldLeft[3] =  0.0f;

    worldUp[0] = 0.0f;
    worldUp[1] = 1.0f;
    worldUp[2] = 0.0f;
    worldUp[3] = 0.0f;

    worldForward[0] =  0.0f;
    worldForward[1] =  0.0f;
    worldForward[2] = -1.0f;
    worldForward[3] =  0.0f;
}

uint32_t initializeControlSet(float turnSpeed, float moveSpeed, void (*controlFunction)(PControlSet)) {
    assert(controlSetCount < nodeCountLimit);
    const uint32_t controlSetIndex = controlSetCount;
    controlSetCount++;

    PControlSet controlSet = &controlSets[controlSetIndex];

    controlSet->turnSpeed = turnSpeed;
    controlSet->moveSpeed = moveSpeed;

    glmc_mat4_identity(controlSet->rotation);
    glmc_mat4_identity(controlSet->translation);

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

void firstPersonControl(PControlSet controlSet) {
    vec2 mouseMovement;
    glmc_vec2_scale(mouseDelta, controlSet->moveSpeed, mouseMovement);

    glmc_rotate(controlSet->rotation, mouseMovement[0], worldUp);

    // TODO: Limit vertical rotation on global up and down
    glmc_rotate(controlSet->rotation, mouseMovement[1], worldLeft);

    vec3 keyboardMovement;
    glmc_vec3_scale(primaryKeyboardInput, controlSet->moveSpeed, keyboardMovement);
    glmc_vec3_rotate_m4(controlSet->rotation, keyboardMovement, keyboardMovement);

    glmc_translate(controlSet->translation, keyboardMovement);
}
