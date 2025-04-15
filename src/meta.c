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

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &sceneDescriptorSet, 0, NULL);
}
