#pragma once

#include "pch.h"

typedef struct camera {
    float fieldOfView;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    uint32_t uniformOffset;
} Camera, *PCamera;

typedef struct cameraUniform {
    mat4 transform;
    mat4 view;
    mat4 projection;
    mat4 projectionView; // NOTICE: Premultiplied to save redundant multiplications on all shader units
    vec4 properties; // NOTICE: field of view, aspect ratio, near plane, far plane
} CameraUniform, *PCameraUniform;

extern uint32_t cameraCountLimit;
extern uint32_t cameraCount;
extern PCamera cameras;
extern PCameraUniform cameraUniforms;

uint32_t loadCamera(cgltf_camera *cameraData);
void bindCamera(uint32_t cameraIndex, uint32_t framebufferSetIndex);