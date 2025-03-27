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

void loadMesh(cgltf_mesh *mesh, mat4 transform) {
    debug("\t\tMesh: %s", mesh->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
        assert(primitiveCount < primitiveCountLimit);

        Primitive *primitiveReference = &primitives[primitiveCount];

        loadPrimitive(primitiveReference, &mesh->primitives[primitiveIndex], transform);

        primitiveCount++;
    }
}

void loadNode(cgltf_node *node) {
    debug("\tNode: %s", node->name);

    mat4 transform;
    cgltf_node_transform_world(node, (cgltf_float *)transform);

    if(node->mesh) {
        loadMesh(node->mesh, transform);
    }

    for(cgltf_size childIndex = 0; childIndex < node->children_count; childIndex++) {
        loadNode(node->children[childIndex]);
    }
}

void loadScene(cgltf_scene *scene) {
    debug("Scene: %s", scene->name);

    for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
        loadNode(scene->nodes[nodeIndex]);
    }
}

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
        loadMaterial(&data->materials[materialIndex]);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        loadScene(&data->scenes[sceneIndex]);
    }
}

void freeAsset(cgltf_data *data) {
    cgltf_free(data);
}

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

    cgltf_data *data;

    loadPlayer((vec3){0.0f, 0.0f, 8.0f}, (vec3){0.0f, 1.0f, 0.0f}, (vec3){1.0f, 0.0f, 0.0f});
    loadCamera(M_PI_4, 1.0f, 100.0f);

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

    free(vertexBuffer);
    free(indexBuffer );

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
