#pragma once

#include "pch.h"

struct sceneUniform {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
    vec4 cameraProperties;
} typedef SceneUniform;

extern Primitive *skybox;
extern Primitive *actor;

void updateView();
void updateProjection();
void generateProjectionView();

void initializeScene();
void initializePlayer(vec3 position, vec3 direction, float speed);
void initializeActor(vec3 position, vec3 direction, float speed);
void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);

void updateSkybox();
void updatePlayer();
void updateActor();
void updateCamera();

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
