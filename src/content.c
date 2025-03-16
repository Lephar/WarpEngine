#include "content.h"

#include "helper.h"
#include "file.h"
#include "queue.h"
#include "memory.h"
#include "buffer.h"
#include "primitive.h"

extern VkDevice device;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedMemory;

uint64_t indexCount;
uint64_t vertexCount;
uint64_t indexBufferSize;
uint64_t vertexBufferSize;
uint64_t uniformBufferSize;

const uint64_t indexBufferSizeLimit   = 1L << 30;
const uint64_t vertexBufferSizeLimit  = 1L << 30;

Index *indexBuffer;
Vertex *vertexBuffer;
Uniform *uniformBuffer;

extern const uint32_t materialCountLimit;
extern uint32_t materialCount;
extern Material *materials;

extern const uint32_t primitiveCountLimit;
extern uint32_t primitiveCount;
extern Primitive *primitives;

void loadContent() {
    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;

    uniformBufferSize = sizeof(Uniform);

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);

    assert(indexBuffer);
    assert(vertexBuffer);

    primitiveCount = 0;
    materialCount = 0;

    materials = malloc(materialCountLimit * sizeof(Material));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    loadAsset(CUBEMAP,    "Skybox.gltf");
    loadAsset(STATIONARY, "Terrain.gltf");
    loadAsset(MOVABLE,    "Suzanne.gltf");
    debug("Assets successfully loaded");

    stagingBufferCopy(indexBuffer,  0, 0, indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    free(vertexBuffer);
    free(indexBuffer);
    debug("Index and vertex data copied into device memory");

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    uniformBuffer = mappedSharedMemory;
}

void freeContent() {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        destroyImageView(&materials[materialIndex].baseColor);
        destroyImage(&materials[materialIndex].baseColor);
    }

    free(materials);
    free(primitives);

    debug("Assets freed");
}
