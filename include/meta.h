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
} typedef Camera;

void updateView();
void updateProjection();
void generateViewProjection();

void loadPlayer(vec3 position, vec3 forward, vec3 right);
void loadCamera(float fieldOfView, float nearPlane, float farPlane);
void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
