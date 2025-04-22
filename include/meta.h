#pragma once

#include "pch.h"

typedef struct primitive Primitive;

extern Primitive *skybox;
extern Primitive *actor;

void updateView();
void updateProjection();
void generateViewProjection();

void initializeScene();
void initializePlayer(vec3 playerPosition, vec3 playerDirection, float playerSpeed);
void initializeCamera(float cameraFieldOfView, float cameraNearPlane, float cameraFarPlane);
void initializeActor(vec3 actorPosition, vec3 actorDirection, float actorSpeed);

void updatePlayer();
void updateCamera();
void updateSkybox();

void bindScene(VkCommandBuffer commandBuffer, VkDescriptorSet sceneDescriptorSet);
