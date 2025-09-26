#include "camera.h"

#include "content.h"
#include "framebuffer.h"
#include "window.h"

#include "logger.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

uint32_t cameraCountLimit;
uint32_t cameraCount;
PCamera cameras;
PCameraUniform cameraUniforms;

uint32_t loadCamera(cgltf_camera *cameraData) {
    debug("Camera: %s", cameraData->name);

    if(cameraData->type != cgltf_camera_type_perspective) {
        debug("\tOnly perspective cameras are supported right now, skipping...");
        return UINT32_MAX;
    }

    cgltf_camera_perspective *perspectiveData = &cameraData->data.perspective;

    if(cameraCount >= cameraCountLimit) {
        debug("\tCamera count limit reached, skipping...");
        return UINT32_MAX;
    }

    const uint32_t cameraIndex = cameraCount;
    cameraCount++;

    PCamera camera = &cameras[cameraIndex];
    PCameraUniform cameraUniform = &cameraUniforms[cameraIndex];

    camera->uniformOffset = cameraIndex * cameraUniformAlignment;

    camera->fieldOfView = perspectiveData->yfov;
    camera->aspectRatio = perspectiveData->aspect_ratio; // NOTICE: Will be discarded anyway when binding to framebuffer
    camera->nearPlane   = perspectiveData->znear;
    camera->farPlane    = perspectiveData->zfar; // TODO: Check if it has_zfar

    debug("\tyfov:         %g", camera->fieldOfView);
    debug("\taspect_ratio: %g", camera->aspectRatio);
    debug("\tznear:        %g", camera->nearPlane);
    debug("\tzfar:         %g", camera->farPlane);

    glmc_perspective_rh_zo(camera->fieldOfView, camera->aspectRatio, camera->nearPlane, camera->farPlane, cameraUniform->projection);

    cameraUniform->properties[0] = camera->fieldOfView;
    cameraUniform->properties[1] = camera->aspectRatio;
    cameraUniform->properties[2] = camera->nearPlane;
    cameraUniform->properties[3] = camera->farPlane;

    debug("\tCamera successfully loaded and perspective projection matrix generated");

    return cameraIndex;
}

// WARN: Do not bind a camera to multiple framebuffer sets!
void bindCamera(uint32_t cameraIndex, uint32_t framebufferSetIndex) {
    PCamera camera = &cameras[cameraIndex];
    PCameraUniform cameraUniform = &cameraUniforms[cameraIndex];
    PFramebufferSet framebufferSet = &framebufferSets[framebufferSetIndex];

    camera->framebufferSetIndex = framebufferSetIndex;
    framebufferSet->cameraIndex = cameraIndex;

    camera->aspectRatio = (float) framebufferSet->extent.width / (float) framebufferSet->extent.height;
    cameraUniform->properties[1] = camera->aspectRatio;

    // NOTICE: Do not generate whole projection matrix, just resize according to the new aspect ratio
    if(cameraUniform->projection[0][0] != 0.0f) { // TODO: Is the else condition an error?
        cameraUniform->projection[0][0] = cameraUniform->projection[1][1] / camera->aspectRatio;
    }

    //glmc_perspective_resize_rh_zo(camera->aspectRatio, cameraUniform->projection); // TODO: Use this function instead when they export

    debug("Bound camera %u to framebuffer set %u and resized the perspective projection matrix with the aspect ratio of %g", cameraIndex, framebufferSetIndex, camera->aspectRatio);
}

// TODO: All the functions below will be moved to separate unit
void initializeWorld(const vec3 up) {
    worldUp[0] = up[0];
    worldUp[1] = up[1];
    worldUp[2] = up[2];
}

void initializePlayer(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  playerPosition);
    glmc_vec3_copy(direction, playerDirection);

    glmc_vec3_normalize(playerDirection);

    playerSpeed = speed;
}

void updatePlayer() {
    vec3 left;
    vec3 up;
    vec3 movement;

    glmc_vec3_rotate(playerDirection, mouseDelta[0], worldUp);
    glmc_vec3_crossn(worldUp, playerDirection, left);

    // TODO: Limit vertical rotation on global up and down
    glmc_vec3_rotate(playerDirection, mouseDelta[1], left);
    glmc_vec3_crossn(playerDirection, left, up);

    glmc_vec3_normalize(playerDirection);

    glmc_vec3_scale(freeMovementInput, playerSpeed, movement);

    glmc_vec3_muladds(left,            movement[0], playerPosition);
    glmc_vec3_muladds(up,              movement[1], playerPosition);
    glmc_vec3_muladds(playerDirection, movement[2], playerPosition);
}
