#pragma once

#include "pch.h"

typedef struct camera {
    mat4 view;
    mat4 projection;
    mat4 projectionView;
    vec4 properties; // NOTICE: field of view, aspect ratio, near plane, far plane
} Camera, *PCamera;

extern uint32_t cameraCountLimit;
extern uint32_t cameraCount;
extern PCamera  cameras;

void loadCamera(cgltf_camera *cameraData);
void bindCamera(uint32_t cameraIndex, uint32_t framebufferSetIndex);

void updateView(uint32_t cameraIndex);
void generateProjectionView(uint32_t cameraIndex);
void updateCamera(uint32_t cameraIndex);

void initializeWorld(const vec3 up);
void initializePlayer(vec3 position, vec3 direction, float speed);

void updatePlayer();
