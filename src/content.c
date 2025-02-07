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

size_t indexCount;
size_t vertexCount;

size_t indexBufferSize;
size_t vertexBufferSize;

size_t uniformSize;

Index   *indexBuffer;
Vertex  *vertexBuffer;
Uniform *uniformBuffer;

size_t assetCount;
Asset *assets;

void processAttribute(Primitive *primitive, cgltf_attribute *attribute) {
    debug("\t\t\t\tAttribute: %s", attribute->name);

    cgltf_accessor *accessor = attribute->data;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    void *data = buffer->data + view->offset;

    if(primitive->vertexCount == 0) {
        primitive->vertexCount = accessor->count;
        primitive->vertices = malloc(accessor->count * sizeof(Vertex));

        vertexCount += accessor->count;
    }

    assert(primitive->vertexCount == accessor->count);
    debug("\t\t\t\t\t%lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(attribute->type == cgltf_attribute_type_position) {
        vec3 *positions = data; // TODO: Evaluate other possible data types

        for(cgltf_size positionIndex = 0; positionIndex < accessor->count; positionIndex++) {
            memcpy(primitive->vertices[positionIndex].position, positions[positionIndex], sizeof(vec3));
        }
    } else if(attribute->type == cgltf_attribute_type_texcoord) {
        vec2 *texcoords = data;

        for(cgltf_size texcoordIndex = 0; texcoordIndex < accessor->count; texcoordIndex++) {
            memcpy(primitive->vertices[texcoordIndex].texcoord, texcoords[texcoordIndex], sizeof(vec2));
        }
    } //TODO: Load normal and tangent too
}

Primitive loadPrimitive(cgltf_primitive *primitiveData) {
    debug("\t\t\tPrimitive Type: %d", primitiveData->type);

    cgltf_accessor *accessor = primitiveData->indices;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    void *data = buffer->data + view->offset;

    Primitive primitive = {
        .indexCount = accessor->count,
        .indices = malloc(accessor->count * sizeof(Index)),
        .vertexCount = 0,
        .vertices = NULL,
        .material = {}
    };

    indexCount += accessor->count;
    debug("\t\t\t\t%lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            primitive.indices[dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        memcpy(primitive.indices, data, view->size);
    } // TODO: Can it be something else?

    for(cgltf_size attributeIndex = 0; attributeIndex < primitiveData->attributes_count; attributeIndex++) {
        cgltf_attribute *attribute = &primitiveData->attributes[attributeIndex];
        processAttribute(&primitive, attribute);
    }

    return primitive;
}

Mesh loadMesh(cgltf_mesh *meshData) {
    debug("\t\tMesh: %s", meshData->name);

    Mesh mesh = {
        .primitiveCount = meshData->primitives_count,
        .primitives = malloc(meshData->primitives_count * sizeof(Primitive))
    };

    for(cgltf_size primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
        cgltf_primitive *primitive = &meshData->primitives[primitiveIndex];
        mesh.primitives[primitiveIndex] = loadPrimitive(primitive);
    }

    return mesh;
}

Node loadNode(cgltf_node *nodeData) {
    debug("\tNode: %s", nodeData->name);

    Node node = {
        //.transform = NULL, // TODO: Initialize with nodeData->matrix
        //.mesh = NULL, // TODO: Initialize here maybe?
        .children = malloc(nodeData->children_count * sizeof(Node))
    };

    if(nodeData->mesh) {
        node.mesh = loadMesh(nodeData->mesh);
    }

    for(cgltf_size childIndex = 0; childIndex < nodeData->children_count; childIndex++) {
        cgltf_node *childNode = nodeData->children[childIndex];
        node.children[childIndex] = loadNode(childNode);
    }

    return node;
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

    return scene;
}

Asset loadAsset(const char *relativePath) {
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

    Asset asset = {
        .sceneCount = data->scenes_count,
        .scenes = malloc(data->scenes_count * sizeof(Scene))
    };

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        cgltf_scene *scene = &data->scenes[sceneIndex];
        asset.scenes[sceneIndex] = loadScene(scene);
    }

    cgltf_free(data);

    return asset;
}

void initializeAssets() {
    debug("Initializing assets");

    indexCount = 0;
    vertexCount = 0;

    indexBufferSize = 0;
    vertexBufferSize = 0;

    uniformSize = 0;

    assetCount = 1;
    assets = malloc(assetCount * sizeof(Asset));

    debug("Started reading asset files");

    assets[0] = loadAsset("assets/Lantern.gltf");

    debug("Assets loaded from files");

    indexBufferSize  = indexCount  * sizeof(Index);
    vertexBufferSize = vertexCount * sizeof(Vertex);

    debug("Allocating host index vertex buffers: %lu and %lu bytes respectively", indexBufferSize, vertexBufferSize);

    indexBuffer  = malloc(indexBufferSize);
    vertexBuffer = malloc(vertexBufferSize);


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
