#include "scene.h"

#include "window.h"
#include "surface.h"
#include "pipeline.h"
#include "framebuffer.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

CameraUniform cameraUniform;

void updateView() {
    vec3 target;

    glmc_vec3_add(playerPosition, playerDirection, target);
    glmc_lookat_rh_zo(playerPosition, target, worldUp, cameraUniform.view);
}

void updateProjection() {
    float aspectRatio = ((float) surfaceExtent.width) / ((float) surfaceExtent.height);
    glmc_perspective_rh_zo(cameraUniform.properties[0], aspectRatio, cameraUniform.properties[1], cameraUniform.properties[2], cameraUniform.projection);
}

void generateProjectionView() {
    glmc_mat4_mul(cameraUniform.projection, cameraUniform.view, cameraUniform.viewProjection);
}

void initializeScene() {
    worldUp[0] = 0.0f;
    worldUp[1] = 1.0f;
    worldUp[2] = 0.0f;
}

void initializePlayer(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  playerPosition);
    glmc_vec3_copy(direction, playerDirection);

    glmc_vec3_normalize(playerDirection);

    playerSpeed = speed;
}

void initializeCamera(float fieldOfView, float nearPlane, float farPlane) {
    cameraUniform.properties[0] = fieldOfView;
    cameraUniform.properties[1] = nearPlane;
    cameraUniform.properties[2] = farPlane;
    cameraUniform.properties[3] = 1.0f; // Placeholder
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

void updateCamera() {
    updateView();
    generateProjectionView();
}

void bindScene(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &framebuffer->cameraDescriptorSet, 0, nullptr);
}
