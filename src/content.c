#include "content.h"

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
    //uniformBuffer = calloc(1, uniformBufferSize);

    indexBuffer[0] = 0;
    indexBuffer[1] = 1;
    indexBuffer[2] = 2;

    vertexBuffer[0].x =  0.0f ;
    vertexBuffer[0].y = -0.5f ;
    vertexBuffer[0].z =  1.0f ;
    vertexBuffer[1].x =  0.5f ;
    vertexBuffer[1].y =  0.5f ;
    vertexBuffer[1].z =  1.0f ;
    vertexBuffer[2].x = -0.5f ;
    vertexBuffer[2].y =  0.5f ;
    vertexBuffer[2].z =  1.0f ;
    /*
    uniformBuffer->transform[0][0] = 1.0f;
    uniformBuffer->transform[1][1] = 1.0f;
    uniformBuffer->transform[2][2] = 1.0f;
    uniformBuffer->transform[3][3] = 1.0f;
    */
    debug("Assets initialized");
}

void loadAssets() {
    VkDeviceSize stagingBufferOffset = uniformBufferSize;

    memcpy(mappedSharedMemory + stagingBufferOffset, indexBuffer, indexBufferSize);
    memcpy(mappedSharedMemory + stagingBufferOffset + indexBufferSize, vertexBuffer, vertexBufferSize);

    copyBuffer(&sharedBuffer, &deviceBuffer, stagingBufferOffset, 0, indexBufferSize + vertexBufferSize);
    memset(mappedSharedMemory, 0, sharedMemory.size);

    //memcpy(mappedSharedMemory, uniformBuffer, uniformBufferSize);
    uniformBuffer = mappedSharedMemory; // TODO: Directly write into shared memory

    debug("Assets copied to device buffer");
}

void freeAssets() {
    //free(uniformBuffer);
    free( vertexBuffer);
    free(  indexBuffer);

    debug("Assets freed");
}
