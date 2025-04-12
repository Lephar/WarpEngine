#pragma once

#include "pch.h"

struct player {
    vec3 position;
    vec3 forward;
    vec3 right;
} typedef Player;

struct camera {
    float fieldOfView;
    float nearPlane;
    float farPlane;

    VkDeviceSize viewOffset;
    VkDeviceSize projOffset;
    VkDeviceSize combOffset;
} typedef Camera;

void loadPlayer(vec3 position, vec3 forward, vec3 right);
void loadCamera(float fieldOfView, float nearPlane, float farPlane);
void loadSkybox(cgltf_data *data);
void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
