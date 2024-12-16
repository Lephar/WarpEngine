#include "element.h"

#include "helper.h"
#include "memory.h"
#include "buffer.h"

uint32_t  indexCount;
uint32_t vertexCount;

VkDeviceSize   indexBufferSize;
VkDeviceSize  vertexBufferSize;
VkDeviceSize uniformBufferSize;

Index    *  indexBuffer;
Vertex   * vertexBuffer;
Uniform  *uniformBuffer;

extern VkDevice device;

extern Memory deviceMemory;
extern Memory sharedMemory;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedMemory;

// TODO: Implement GLTF loading
void initializeAssets() {
     indexCount = 3;
    vertexCount = 3;

      indexBufferSize =  indexCount * sizeof(Index  );
     vertexBufferSize = vertexCount * sizeof(Vertex );
    uniformBufferSize =               sizeof(Uniform);

      indexBuffer = malloc(           indexBufferSize);
     vertexBuffer = malloc(          vertexBufferSize);
    uniformBuffer = calloc(1, uniformBufferSize);

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

    uniformBuffer->transform[0][0] = 1.0f;
    uniformBuffer->transform[1][1] = 1.0f;
    uniformBuffer->transform[2][2] = 1.0f;
    uniformBuffer->transform[3][3] = 1.0f;

    debug("Assets initialized");
}

void loadAssets() {
    memcpy(mappedSharedMemory, indexBuffer, indexBufferSize);
    memcpy(mappedSharedMemory + indexBufferSize, vertexBuffer, vertexBufferSize);

    copyBuffer(&sharedBuffer, &deviceBuffer, 0, 0, indexBufferSize + vertexBufferSize);

    memcpy(mappedSharedMemory, uniformBuffer, uniformBufferSize);

    debug("Assets copied to device buffer");
}

void freeAssets() {
    free(uniformBuffer);
    free( vertexBuffer);
    free(  indexBuffer);

    debug("Assets freed");
}
