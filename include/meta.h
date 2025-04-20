#pragma once

#include "pch.h"

typedef struct primitive Primitive;

extern Primitive *skybox;

void updateView();
void updateProjection();
void generateViewProjection();

void loadPlayer(vec3 playerPosition, vec3 playerDirection, float playerSpeed);
void loadCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);
void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);

void updatePlayer();
void updateCamera();
void updateSkybox();
