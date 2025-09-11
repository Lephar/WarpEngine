#include "scene.h"

#include "window.h"
#include "pipeline.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

SceneUniform sceneUniform;

void updateView() {
    vec3 target;

    glmc_vec3_add(playerPosition, playerDirection, target);
    glmc_lookat_rh_zo(playerPosition, target, worldUp, sceneUniform.view);
}

void updateProjection() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(sceneUniform.cameraProperties[0], aspectRatio, sceneUniform.cameraProperties[1], sceneUniform.cameraProperties[2], sceneUniform.projection);
}

void generateProjectionView() {
    glmc_mat4_mul(sceneUniform.projection, sceneUniform.view, sceneUniform.viewProjection);
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
    sceneUniform.cameraProperties[0] = fieldOfView;
    sceneUniform.cameraProperties[1] = nearPlane;
    sceneUniform.cameraProperties[2] = farPlane;
    sceneUniform.cameraProperties[3] = 1.0f; // Placeholder
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

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 3, 1, &sceneDescriptorSet, 0, nullptr);
}
