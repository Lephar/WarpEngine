#pragma once

#include "pch.h"

typedef struct node {
    char name[UINT8_MAX];
    uint32_t controlSetIndex;
    mat4 scale;
    mat4 rotation;
    mat4 translation;
    uint32_t cameraIndex;
    uint32_t meshCount;
    uint32_t *meshIndices;
    uint32_t childCount;
    uint32_t *childrenIndices;
} Node, *PNode;

extern uint32_t nodeCountLimit;
extern uint32_t nodeCount;
extern PNode nodes;

extern uint32_t sceneCount;
extern uint32_t *scenes; // Indices to the scene root nodes

void loadAsset(const char *subdirectory, const char *filename);
void updateNodeUniformBuffer(PNode node, mat4 transform);
