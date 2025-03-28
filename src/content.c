#include "content.h"

#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "meta.h"
#include "primitive.h"

#include "file.h"
#include "logger.h"

uint64_t indexCount;
uint64_t vertexCount;

uint64_t indexBufferSize;
uint64_t vertexBufferSize;
uint64_t uniformBufferSize;

Index  *indexBuffer;
Vertex *vertexBuffer;
void   *uniformBuffer;

cgltf_data *loadAsset(const char *name) {
    char fullPath[PATH_MAX];
    makeFullPath("data", name, fullPath);

    cgltf_data *data = NULL;
    cgltf_options assetOptions = {};
    cgltf_result result;

    result = cgltf_parse_file(&assetOptions, fullPath, &data);

    if(result != cgltf_result_success) {
        debug("Failed to read %s: %d", name, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(data);

    if(result != cgltf_result_success) {
        debug("Failed to validate %s: %d", name, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_load_buffers(&assetOptions, data, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", name, result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    return data;
}

void processAsset(cgltf_data *data) {
    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        cgltf_material *materialData = &data->materials[materialIndex];

        if(findMaterial(materialData) < materialCount) {
            continue;
        }

        assert(materialCount < materialCountLimit);
        Material *material = &materials[materialCount];

        loadMaterial(material, materialData);
        materialCount++;
    }

    for(cgltf_size nodeIndex = 0; nodeIndex < data->nodes_count; nodeIndex++) {
        cgltf_node *nodeData = &data->nodes[nodeIndex];

        if(nodeData->mesh) {
            mat4 transform;
            cgltf_node_transform_world(nodeData, (cgltf_float *) transform);

            cgltf_mesh *meshData = nodeData->mesh;

            for(cgltf_size primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
                assert(primitiveCount < primitiveCountLimit);
                Primitive *primitiveReference = &primitives[primitiveCount];

                loadPrimitive(primitiveReference, &meshData->primitives[primitiveIndex], transform);
                primitiveCount++;
            }
        }
    }
}

void freeAsset(cgltf_data *data) {
    cgltf_free(data);
}

void createContentBuffers() {
    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;
    uniformBufferSize = 0;

    const uint64_t indexBufferSizeLimit   = deviceMemory.size / 2;
    const uint64_t vertexBufferSizeLimit  = deviceMemory.size / 2;
    const uint64_t uniformBufferSizeLimit = sharedMemory.size;

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);
    uniformBuffer = malloc(uniformBufferSizeLimit);

    assert(indexBuffer);
    assert(vertexBuffer);
    assert(uniformBuffer);

    materialCount  = 0;
    primitiveCount = 0;

    materials  = malloc(materialCountLimit  * sizeof(Material ));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    debug("Content buffers created");
}

void loadContent() {
    vec3 position = {0.0f, 0.0f, 8.0f};
    vec3 forward  = {0.0f, 1.0f, 0.0f};
    vec3 right    = {1.0f, 0.0f, 0.0f};

    loadPlayer(position, forward, right);

    float fieldOfView = M_PI_4;
    float nearPlane   =   1.0f;
    float farPlane    = 100.0f;

    loadCamera(fieldOfView, nearPlane, farPlane);

    debug("Meta materials successfully loaded");

    cgltf_data *data;

    data = loadAsset("Skybox.gltf");
    loadSkybox(data);
    freeAsset(data);

    data = loadAsset("Terrain.gltf");
    processAsset(data);
    freeAsset(data);

    data = loadAsset("Suzanne.gltf");
    processAsset(data);
    freeAsset(data);

    debug("Assets successfully loaded");

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    debug("Index and vertex data copied into device memory");
}

void destroyContentBuffers() {
    free(uniformBuffer);
    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void freeContent() {
    free(primitives);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        destroyMaterial(&materials[materialIndex]);
    }

    free(materials);
}
