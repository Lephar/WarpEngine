#include "manager.h"

#include "memory.h"
#include "buffer.h"

uint32_t  indexCount;
uint32_t vertexCount;

Index    *  indexBuffer;
Vertex   * vertexBuffer;
Uniform  *uniformBuffer;

extern VkDevice device;

extern Memory deviceMemory;
extern Memory sharedMemory;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

// TODO: Implement GLTF loading
void initializeAssets() {
    indexCount  = 3;
    vertexCount = 3;

      indexBuffer = malloc( indexCount * sizeof(Index  ));
     vertexBuffer = malloc(vertexCount * sizeof(Vertex ));
    uniformBuffer = calloc(1, sizeof(Uniform));

    indexBuffer[0] = 0;
    indexBuffer[1] = 1;
    indexBuffer[2] = 2;

    vertexBuffer[0].x =  0.0f ;
    vertexBuffer[0].y = -0.67f;
    vertexBuffer[0].z =  0.5f ;
    vertexBuffer[1].x =  0.5f ;
    vertexBuffer[1].y =  0.5f ;
    vertexBuffer[1].z =  0.5f ;
    vertexBuffer[2].x = -0.5f ;
    vertexBuffer[2].y =  0.5f ;
    vertexBuffer[2].z =  0.5f ;

    uniformBuffer->transform[0][0] = 1;
    uniformBuffer->transform[1][1] = 1;
    uniformBuffer->transform[2][2] = 1;
    uniformBuffer->transform[3][3] = 1;
}

void loadAssets() {
}

void freeAssets() {
}