#include "content.h"

#include "helper.h"
#include "file.h"
#include "memory.h"
#include "buffer.h"

uint32_t  indexCount;
uint32_t vertexCount;

VkDeviceSize   indexBufferSize;
VkDeviceSize  vertexBufferSize;
VkDeviceSize uniformBufferSize;

VkDeviceSize  indexBufferBegin;
VkDeviceSize vertexBufferBegin;

Index   *  indexBuffer;
Vertex  * vertexBuffer;
Uniform *uniformBuffer;

extern VkDevice device;

extern Memory deviceMemory;
extern Memory sharedMemory;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedMemory;

void processAttribute(cgltf_attribute *attribute) {
    debug("\t\t\t\tAttribute: %s, %d", attribute->name, attribute->type);

    cgltf_accessor *accessor = attribute->data;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    uint16_t *data = buffer->data + view->offset;

    if(attribute->type == cgltf_attribute_type_position) {
        memcpy(mappedSharedMemory + vertexBufferBegin + vertexBufferSize, data, view->size);

        vertexBufferSize += view->size;
        vertexCount += accessor->count;

        debug("\t\t\t\t\t%lu elements copied, %lu bytes in size", accessor->count, view->size);
    }
}

void processPrimitive(cgltf_primitive *primitive) {
    debug("\t\t\tPrimitive Type: %d", primitive->type);

    cgltf_accessor *accessor = primitive->indices;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    void *data = buffer->data + view->offset;

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            //mesh.indices[dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        //memcpy(mesh.indices, data, view->size);
    }

    indexBufferSize += view->size;
    indexCount += accessor->count;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitive->attributes_count; attributeIndex++) {
        cgltf_attribute *attribute = &primitive->attributes[attributeIndex];
        processAttribute(attribute);
    }
}

void loadMesh(cgltf_mesh *mesh) {
    debug("\t\tMesh:%s", mesh->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
        cgltf_primitive *primitive = &mesh->primitives[primitiveIndex];
        processPrimitive(primitive);
    }
}

void loadNode(cgltf_node *node) {
    debug("\tNode:%s", node->name);

    if(node->mesh)
        loadMesh(node->mesh);

    for(cgltf_size childIndex = 0; childIndex < node->children_count; childIndex++) {
        cgltf_node *childNode = node->children[childIndex];
        loadNode(childNode);
    }
}

void loadScene(cgltf_scene *scene) {
    debug("Scene: %s", scene->name);

    for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
        cgltf_node *node = scene->nodes[nodeIndex];
        loadNode(node);
    }
}

void loadModel(Model *model) {
    cgltf_data* data = NULL;
    cgltf_options assetOptions = {};

    cgltf_result result;
    //cgltf_parse_file(&assetOptions, model->fullpath, &data);

    if(result != cgltf_result_success) {
        debug("Failed to read %s: %d", result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(data);

    if(result != cgltf_result_success) {
        debug("Failed to validate %s: %d", result);
        assert(result == cgltf_result_success);
    }

    // TODO: Load materials here

    //result = cgltf_load_buffers(&assetOptions, data, model->fullpath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        cgltf_scene *scene = &data->scenes[sceneIndex];
        loadScene(scene);
    }

    cgltf_free(data);
}

void initializeAssets() {
    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;
    uniformBufferSize = sizeof(Uniform);

    indexBufferBegin  = uniformBufferSize;
    vertexBufferBegin = indexBufferBegin + sharedMemory.size / 2;
    /*

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
    */

    loadModel("assets/Lantern.gltf");

    debug("Assets initialized");
}

void loadAssets() {
    //memcpy(mappedSharedMemory + stagingBufferOffset, indexBuffer, indexBufferSize);
    //memcpy(mappedSharedMemory + stagingBufferOffset + indexBufferSize, vertexBuffer, vertexBufferSize);

    //copyBuffer(&sharedBuffer, &deviceBuffer, indexBufferBegin, 0, indexBufferSize + vertexBufferSize);
    copyBuffer(&sharedBuffer, &deviceBuffer, indexBufferBegin, 0, sharedBuffer.size - indexBufferBegin);
    memset(mappedSharedMemory, 0, sharedMemory.size);

    uniformBuffer = mappedSharedMemory; // TODO: Directly write into shared memory

    debug("Assets copied to device buffer");
}

void freeAssets() {
    //free( vertexBuffer);
    //free(  indexBuffer);

    free(models); // NOTICE: Allocated in config unit

    debug("Assets freed");
}
