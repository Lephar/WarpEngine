#include "content.h"

#include "helper.h"
#include "memory.h"
#include "buffer.h"

uint32_t modelCount;
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

void loadNode(cgltf_node *node) {
    debug("%s %s", node->name, node->mesh ? node->mesh->name : "");

    for(uint32_t childIndex = 0; childIndex < node->children_count; childIndex++) {
        cgltf_node *childNode = node->children[childIndex];
        loadNode(childNode);
    }
}

void loadModel(Model *model) {
    cgltf_data* data = NULL;
    cgltf_options assetOptions = {};

    cgltf_result result = cgltf_parse_file(&assetOptions, model->fullpath, &data);

    if (result != cgltf_result_success) {
        debug("Failed to read %s: %d", result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(data);

    if (result != cgltf_result_success) {
        debug("Failed to validate %s: %d", result);
        assert(result == cgltf_result_success);
    }

    // TODO: Load materials here

    result = cgltf_load_buffers(&assetOptions, data, model->fullpath);

    if (result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    for (uint32_t sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        cgltf_scene *scene = &data->scenes[sceneIndex];

        for (uint32_t nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
            cgltf_node *node = scene->nodes[nodeIndex];
            loadNode(node);
        }
    }

    cgltf_free(data);
}

// TODO: Implement GLTF loading
void initializeAssets() {
    for(uint32_t modelIndex = 0; modelIndex < modelCount; modelIndex++) {
        loadModel(&models[modelIndex]);
    }


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
