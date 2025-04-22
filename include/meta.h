#pragma once

#include "pch.h"

typedef struct primitive Primitive;

extern Primitive *skybox;

void updateView();
void updateProjection();
void generateViewProjection();

void initializeScene();
void initializePlayer(vec3 playerPosition, vec3 playerDirection, float playerSpeed);
void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);

void updatePlayer();
void updateCamera();
void updateSkybox();

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
