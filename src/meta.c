#include "meta.h"

#include "window.h"
#include "pipeline.h"
#include "content.h"
#include "material.h"
#include "primitive.h"

#include "logger.h"

vec3 worldUp;

Player player;
Camera camera;

void updateView() {
    vec3 target;
    glmc_vec3_add(player.position, player.forward, target);
    glmc_lookat_rh_zo(player.position, target, worldUp, sceneUniform->view);
}

void updateProjection() {
    float aspectRatio = ((float) extent.width) / ((float) extent.height);
    glmc_perspective_rh_zo(camera.fieldOfView, aspectRatio, camera.nearPlane, camera.farPlane, sceneUniform->projection);
}

void generateViewProjection() {
    glmc_mat4_mul(sceneUniform->projection, sceneUniform->view, sceneUniform->viewProjection);
}

void loadPlayer(vec3 position, vec3 forward, vec3 right) {
    glm_vec3_copy(position, player.position);
    glm_vec3_copy(forward,  player.forward);
    glm_vec3_copy(right,    player.right);
}

void loadCamera(float fieldOfView, float nearPlane, float farPlane) {
    camera.fieldOfView = fieldOfView;
    camera.nearPlane   = nearPlane;
    camera.farPlane    = farPlane;

    updateView();
    updateProjection();
    generateViewProjection();
}

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet) {
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &sceneDescriptorSet, 0, NULL);
}
