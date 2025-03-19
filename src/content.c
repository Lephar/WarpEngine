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

    indexBufferSize  = 0;
    vertexBufferSize = 0;

    uniformBufferSize = sizeof(Uniform);

    const uint64_t indexBufferSizeLimit   = 1L << 30;
    const uint64_t vertexBufferSizeLimit  = 1L << 30;

    indexBuffer  = malloc(indexBufferSizeLimit);
    vertexBuffer = malloc(vertexBufferSizeLimit);

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
    uniformBuffer = mappedSharedMemory;
    debug("Shared memory cleared and set for uniform buffer usage");
}
