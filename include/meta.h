#pragma once

#include "pch.h"

typedef struct primitive Primitive;

extern Primitive *skybox;
extern Primitive *actor;

void updateView();
void updateProjection();
void generateViewProjection();

void initializeScene();
void initializePlayer(vec3 position, vec3 direction, float speed);
void initializeActor(vec3 position, vec3 direction, float speed);
void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);

void updateSkybox();
void updatePlayer();
void updateActor();
void updateCamera();

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
