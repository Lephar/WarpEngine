#include "meta.h"

#include "window.h"
#include "pipeline.h"
#include "content.h"
#include "material.h"
#include "primitive.h"

#include "logger.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

vec3 actorPosition;
vec3 actorDirection;

float actorSpeed;

float fieldOfView;

float nearPlane;
float farPlane;

Primitive *skybox;
Primitive *actor;

void updateView() {
    vec3 target;

    glmc_vec3_add(playerPosition, playerDirection, target);
    glmc_lookat_rh_zo(playerPosition, target, worldUp, sceneUniform->view);
}

void updateProjection() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(fieldOfView, aspectRatio, nearPlane, farPlane, sceneUniform->projection);
}

void generateViewProjection() {
    glmc_mat4_mul(sceneUniform->projection, sceneUniform->view, sceneUniform->viewProjection);
}

void initializeScene() {
    worldUp[0] = 0.0f;
    worldUp[1] = 0.0f;
    worldUp[2] = 1.0f;

    skybox = &primitives[0];
    actor  = &primitives[primitiveCount - 1];
}

void initializePlayer(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  playerPosition);
    glmc_vec3_copy(direction, playerDirection);

    playerSpeed = speed;
}

void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane) {
    fieldOfView = cameraFieldOfView;
    nearPlane   = cameraNearPlane;
    farPlane    = cameraFarPlane;
}

void initializeActor(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  actorPosition);
    glmc_vec3_copy(direction, actorDirection);

    actorSpeed = speed;
}

void updatePlayer() {
    vec3 right;
    vec3 up;
    vec3 movement;

    glmc_vec3_rotate(playerDirection, mouseDelta[0], worldUp);
    glmc_vec3_crossn(playerDirection, worldUp, right);

    // TODO: Limit vertical rotation on global up and down
    glmc_vec3_rotate(playerDirection, mouseDelta[1], right);
    glmc_vec3_crossn(right, playerDirection, up);

    glmc_vec3_normalize(playerDirection);

    glmc_vec3_scale(freeMovementInput, playerSpeed, movement);

    glmc_vec3_muladds(right,     movement[0], playerPosition);
    glmc_vec3_muladds(playerDirection, movement[1], playerPosition);
    glmc_vec3_muladds(up,        movement[2], playerPosition);
}

void updateCamera() {
    updateView();
    generateViewProjection();
}

void updateSkybox() {
    glmc_translate_make(uniformBuffer + skybox->uniformOffset, playerPosition);
}

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &sceneDescriptorSet, 0, NULL);
}
