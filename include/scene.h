#pragma once

#include "pch.h"

typedef struct sceneUniform {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraProperties;
} SceneUniform, *PSceneUniform;

extern SceneUniform sceneUniform;

void updateView();
void updateProjection();
void generateProjectionView();

void initializeScene();
void initializePlayer(vec3 position, vec3 direction, float speed);
void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);

void updatePlayer();
void updateCamera();

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
