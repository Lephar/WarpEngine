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

Index *indexBuffer;
Vertex *vertexBuffer;
Uniform *uniformBuffer;

void loadContent() {
    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;
    uniformBufferSize = 0;

    const uint64_t indexBufferSizeLimit    = deviceMemory.size / 2;
    const uint64_t vertexBufferSizeLimit   = deviceMemory.size / 2;
    const uint64_t uniformBufferSizeLimit  = sharedMemory.size;

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);
    uniformBuffer = malloc(uniformBufferSizeLimit);

    assert(indexBuffer);
    assert(vertexBuffer);

    materialCount  = 0;
    primitiveCount = 0;

    materials  = malloc(materialCountLimit  * sizeof(Material ));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    loadAsset(CUBEMAP,    "Skybox.gltf");
    loadAsset(STATIONARY, "Terrain.gltf");
    loadAsset(MOVABLE,    "Suzanne.gltf");
    debug("Assets successfully loaded");

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);
    debug("Index and vertex data copied into device memory");

    free(vertexBuffer);
    free(indexBuffer );

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void freeContent() {
    for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
        destroyPrimitive(&primitives[primitiveIndex]);
    }

    free(primitives);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        destroyMaterial(&materials[materialIndex]);
    }

    free(materials);
}
