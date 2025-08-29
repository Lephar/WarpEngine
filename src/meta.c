#include "meta.h"

#include "window.h"
#include "pipeline.h"
#include "content.h"
#include "primitive.h"

vec3 worldUp;

vec3 playerPosition;
vec3 playerDirection;

float playerSpeed;

vec3 actorPosition;
vec3 actorDirection;

float actorSpeed;

float cameraFieldOfView;

float cameraNearPlane;
float cameraFarPlane;

Primitive *skybox;
Primitive *actor;

void updateView() {
    vec3 target;

    glmc_vec3_add(playerPosition, playerDirection, target);
    glmc_lookat_rh_zo(playerPosition, target, worldUp, sceneUniform->view);
}

void updateProjection() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(cameraFieldOfView, aspectRatio, cameraNearPlane, cameraFarPlane, sceneUniform->projection);

    sceneUniform->cameraProperties[0] = cameraFieldOfView;
    sceneUniform->cameraProperties[1] = cameraNearPlane;
    sceneUniform->cameraProperties[2] = cameraFarPlane;
    sceneUniform->cameraProperties[3] = 1.0f; // Placeholder
}

void generateProjectionView() {
    glmc_mat4_mul(sceneUniform->projection, sceneUniform->view, sceneUniform->viewProjection);
}

void initializeScene() {
    worldUp[0] = 0.0f;
    worldUp[1] = 1.0f;
    worldUp[2] = 0.0f;

    //skybox = &primitives[0];
    //actor  = &primitives[primitiveCount - 1];
}

void initializePlayer(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  playerPosition);
    glmc_vec3_copy(direction, playerDirection);

    glmc_vec3_normalize(playerDirection);

    playerSpeed = speed;
}

void initializeActor(vec3 position, vec3 direction, float speed) {
    glmc_vec3_copy(position,  actorPosition);
    glmc_vec3_copy(direction, actorDirection);

    glmc_vec3_normalize(actorDirection);

    actorSpeed = speed;
}

void initializeCamera(float fieldOfView, float nearPlane, float farPlane) {
    cameraFieldOfView = fieldOfView;
    cameraNearPlane   = nearPlane;
    cameraFarPlane    = farPlane;
}

void updateSkybox() {
    PrimitiveUniform *skyboxUniform = uniformBuffer + skybox->uniformOffset;
    glmc_translate_make(skyboxUniform->model, playerPosition);
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

void updateActor() {
    vec3 left;
    vec3 movement;

    glmc_vec3_crossn(worldUp, actorDirection, left);
    glmc_vec3_scale(mainMovementInput, actorSpeed, movement);

    glmc_vec3_muladds(left,           movement[0], actorPosition);
    glmc_vec3_muladds(worldUp,        movement[1], actorPosition);
    glmc_vec3_muladds(actorDirection, movement[2], actorPosition);

    PrimitiveUniform *actorUniform = uniformBuffer + actor->uniformOffset;
    glmc_translate_make(actorUniform->model, actorPosition);
}

void updateCamera() {
    updateView();
    generateProjectionView();
}

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &sceneDescriptorSet, 0, NULL);
}
