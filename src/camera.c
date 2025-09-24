#include "camera.h"

#include "framebuffer.h"
#include "window.h"

#include "logger.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

uint32_t cameraCountLimit;
uint32_t cameraCount;
PCamera  cameras;

void loadCamera(cgltf_camera *cameraData) {
    debug("Camera: %s", cameraData->name);

    if(cameraData->type != cgltf_camera_type_perspective) {
        debug("\tOnly perspective cameras are supported right now, skipping...");
        return;
    }

    cgltf_camera_perspective *perspectiveData = &cameraData->data.perspective;

    if(cameraCount >= cameraCountLimit) {
        debug("\tCamera count limit reached, skipping...");
        return;
    }

    const uint32_t cameraIndex = cameraCount;
    cameraCount++;

    PCamera camera = &cameras[cameraIndex];

    camera->properties[0] = perspectiveData->yfov;
    camera->properties[1] = perspectiveData->aspect_ratio; // NOTICE: Will be discarded anyway when binding to framebuffer
    camera->properties[2] = perspectiveData->znear;
    camera->properties[3] = perspectiveData->zfar; // TODO: Check if it has_zfar

    debug("\tyfov:         %g", camera->properties[0]);
    debug("\taspect_ratio: %g", camera->properties[1]);
    debug("\tznear:        %g", camera->properties[2]);
    debug("\tzfar:         %g", camera->properties[3]);

    debug("\tSuccessfully loaded");
}

void updateView(uint32_t framebufferSetIndex) {
    if(framebufferSetIndex >= cameraCount) {
        return;
    }

    PCamera camera = &cameras[framebufferSetIndex];

    vec3 target;

    glmc_vec3_add(playerPosition, playerDirection, target);
    glmc_lookat_rh_zo(playerPosition, target, worldUp, camera->view);
}

void updateProjection(uint32_t framebufferSetIndex) {
    if(framebufferSetIndex >= cameraCount) {
        return;
    }

    PCamera camera = &cameras[framebufferSetIndex];
    PFramebufferSet framebufferSet = &framebufferSets[framebufferSetIndex];

    camera->properties[1] = (float) framebufferSet->extent.width / (float) framebufferSet->extent.height;
    glmc_perspective_rh_zo(camera->properties[0], camera->properties[1], camera->properties[2], camera->properties[3], camera->projection);

    debug("Updated the perspective projection with the aspect ratio of %g", camera->properties[1]);
}

void generateProjectionView(uint32_t framebufferSetIndex) {
    if(framebufferSetIndex >= cameraCount) {
        return;
    }

    PCamera camera = &cameras[framebufferSetIndex];
    glmc_mat4_mul(camera->projection, camera->view, camera->projectionView);
}

void initializeWorld(vec3 up) {
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

void updateCamera(uint32_t framebufferSetIndex) {
    if(framebufferSetIndex >= cameraCount) {
        return;
    }

    updateView(framebufferSetIndex);
    generateProjectionView(framebufferSetIndex);
}
