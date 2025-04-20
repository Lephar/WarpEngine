#include "meta.h"

#include "window.h"
#include "pipeline.h"
#include "content.h"
#include "material.h"
#include "primitive.h"

#include "logger.h"

vec3 worldUp;

vec3 position;
vec3 direction;

float speed;

float fieldOfView;

float nearPlane;
float farPlane;

Primitive *skybox;

void updateView() {
    vec3 target;

    glmc_vec3_add(position, direction, target);
    glmc_lookat_rh_zo(position, target, worldUp, sceneUniform->view);
}

void updateProjection() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(fieldOfView, aspectRatio, nearPlane, farPlane, sceneUniform->projection);
}

void generateViewProjection() {
    glmc_mat4_mul(sceneUniform->projection, sceneUniform->view, sceneUniform->viewProjection);
}

void loadPlayer(vec3 playerPosition, vec3 playerDirection, float playerSpeed) {
    worldUp[0] = 0.0f;
    worldUp[1] = 0.0f;
    worldUp[2] = 1.0f;

    glmc_vec3_copy(playerPosition,  position);
    glmc_vec3_copy(playerDirection, direction);

    speed = playerSpeed;
}

void loadCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane) {
    fieldOfView = cameraFieldOfView;
    nearPlane   = cameraNearPlane;
    farPlane    = cameraFarPlane;
}

void updatePlayer() {
    vec3 right;
    vec3 up;
    vec3 movement;

    glmc_vec3_rotate(direction, mouseDelta[0], worldUp);
    glmc_vec3_crossn(direction, worldUp, right);

    // TODO: Limit vertical rotation on global up and down
    glmc_vec3_rotate(direction, mouseDelta[1], right);
    glmc_vec3_crossn(right, direction, up);

    glmc_vec3_normalize(direction);

    glmc_vec3_scale(freeMovementInput, speed, movement);

    glmc_vec3_muladds(right,     movement[0], position);
    glmc_vec3_muladds(direction, movement[1], position);
    glmc_vec3_muladds(up,        movement[2], position);
}

void updateCamera() {
    updateView();
    generateViewProjection();
}

void updateSkybox() {
    glmc_translate_make(uniformBuffer + skybox->uniformOffset, position);
}

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &sceneDescriptorSet, 0, NULL);
}
