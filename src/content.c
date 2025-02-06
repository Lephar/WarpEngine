#include "content.h"

#include "helper.h"
#include "file.h"
#include "memory.h"
#include "buffer.h"

extern VkDevice device;

extern Memory deviceMemory;
extern Memory sharedMemory;

extern Buffer deviceBuffer;
extern Buffer sharedBuffer;

extern void *mappedSharedMemory;

Index   *indexBuffer;
Vertex  *vertexBuffer;
Uniform *uniformBuffer;

size_t modelCount;
Model *models;

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
    //cgltf_buffer *buffer = view->buffer;

    //void *data = buffer->data + view->offset;

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            //mesh->indices[dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        //memcpy(mesh->indices, data, view->size);
    }

    indexBufferSize += view->size;
    indexCount += accessor->count;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitive->attributes_count; attributeIndex++) {
        cgltf_attribute *attribute = &primitive->attributes[attributeIndex];
        processAttribute(attribute);
    }
}

Mesh loadMesh(cgltf_mesh *meshData) {
    debug("\t\tMesh:%s", meshData->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
        cgltf_primitive *primitive = &meshData->primitives[primitiveIndex];
        processPrimitive(primitive);
    }
}

Node loadNode(cgltf_node *nodeData) {
    debug("\tNode:%s", nodeData->name);

    if(nodeData->mesh) {
        loadMesh(nodeData->mesh);
    }

    for(cgltf_size childIndex = 0; childIndex < nodeData->children_count; childIndex++) {
        cgltf_node *childNode = nodeData->children[childIndex];
        loadNode(childNode);
    }
}

Scene loadScene(cgltf_scene *sceneData) {
    debug("Scene: %s", sceneData->name);

    Scene scene = {
        .nodeCount = sceneData->nodes_count,
        .nodes = malloc(sceneData->nodes_count * sizeof(Node))
    };

    for (cgltf_size nodeIndex = 0; nodeIndex < sceneData->nodes_count; nodeIndex++) {
        cgltf_node *node = sceneData->nodes[nodeIndex];
        scene.nodes[nodeIndex] = loadNode(node);
    }
}

Model loadModel(const char *relativePath) {
    char fullPath[PATH_MAX];
    makeFullPath(relativePath, fullPath);

    cgltf_data* data = NULL;
    cgltf_options assetOptions = {};

    cgltf_result result;
    result = cgltf_parse_file(&assetOptions, fullPath, &data);

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

    result = cgltf_load_buffers(&assetOptions, data, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    Model model = {
        .sceneCount = data->scenes_count,
        .scenes = malloc(data->scenes_count * sizeof(Scene))
    };

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        cgltf_scene *scene = &data->scenes[sceneIndex];
        model.scenes[sceneIndex] = loadScene(scene);
    }

    cgltf_free(data);

    return model;
}

void initializeAssets() {
    modelCount = 1;
    models = malloc(modelCount * sizeof(Model));

    models[0] = loadModel("assets/Lantern.gltf");

    debug("Assets initialized");
}

void loadAssets() {
    copyBuffer(&sharedBuffer, &deviceBuffer, 0, 0, sharedBuffer.size);

    uniformBuffer = mappedSharedMemory; // Directly write into shared memory

    debug("Assets copied to device buffer");
}

void freeAssets() {
    //free( vertexBuffer);
    //free(  indexBuffer);

    //free(models); // NOTICE: Allocated in config unit

    debug("Assets freed");
}
