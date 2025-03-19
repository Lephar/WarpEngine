#include "content.h"

#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "primitive.h"

#include "logger.h"

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
