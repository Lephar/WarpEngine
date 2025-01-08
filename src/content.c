#include "content.h"

#include "helper.h"
#include "memory.h"
#include "buffer.h"

Model *models;

uint32_t  indexCount;
uint32_t vertexCount;

VkDeviceSize   indexBufferSize;
VkDeviceSize  vertexBufferSize;
VkDeviceSize uniformBufferSize;

Index   *  indexBuffer;
Vertex  * vertexBuffer;
Uniform *uniformBuffer;

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

     indexBuffer = malloc( indexBufferSize);
    vertexBuffer = malloc(vertexBufferSize);

    indexBuffer[0] = 0;
    indexBuffer[1] = 1;
    indexBuffer[2] = 2;

    vertexBuffer[0].x =  0.0f ;
    vertexBuffer[0].y =  0.5f ;
    vertexBuffer[0].z =  0.5f ;
    vertexBuffer[1].x = -0.5f ;
    vertexBuffer[1].y =  0.5f ;
    vertexBuffer[1].z = -0.5f ;
    vertexBuffer[2].x =  0.5f ;
    vertexBuffer[2].y =  0.5f ;
    vertexBuffer[2].z = -0.5f ;

    debug("Assets initialized");
}

void loadAssets() {
    VkDeviceSize stagingBufferOffset = uniformBufferSize;

    memcpy(mappedSharedMemory + stagingBufferOffset, indexBuffer, indexBufferSize);
    memcpy(mappedSharedMemory + stagingBufferOffset + indexBufferSize, vertexBuffer, vertexBufferSize);

    copyBuffer(&sharedBuffer, &deviceBuffer, stagingBufferOffset, 0, indexBufferSize + vertexBufferSize);
    memset(mappedSharedMemory, 0, sharedMemory.size);

    uniformBuffer = mappedSharedMemory; // TODO: Directly write into shared memory

    debug("Assets copied to device buffer");
}

void freeAssets() {
    free( vertexBuffer);
    free(  indexBuffer);

    free(models); // NOTICE: Allocated in config unit

    debug("Assets freed");
}
