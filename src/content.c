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

size_t materialCount;
Material *materials;
ProtoMaterial **protoMaterialReferences;

size_t textureBufferSize;
void  *textureBuffer;

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

void processAttribute(Primitive *primitive, cgltf_attribute *attribute, mat4 transform) {
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
            glm_mat4_mulv3(transform, positions[positionIndex], 1.0f, primitive->vertices[positionIndex].position);
        }
    } else if(attribute->type == cgltf_attribute_type_texcoord) {
        vec2 *texcoords = data;

        for(cgltf_size texcoordIndex = 0; texcoordIndex < accessor->count; texcoordIndex++) {
            memcpy(primitive->vertices[texcoordIndex].texcoord, texcoords[texcoordIndex], sizeof(vec2));
        }
    } //TODO: Load normal and tangent too
}

Primitive loadPrimitive(cgltf_primitive *primitiveData, mat4 transform) {
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
        processAttribute(&primitive, attribute, transform);
    }

    return primitive;
}

Mesh loadMesh(cgltf_mesh *meshData, mat4 transform) {
    debug("\t\tMesh: %s", meshData->name);

    Mesh mesh = {
        .primitiveCount = meshData->primitives_count,
        .primitives = malloc(meshData->primitives_count * sizeof(Primitive))
    };

    for(cgltf_size primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
        cgltf_primitive *primitive = &meshData->primitives[primitiveIndex];
        mesh.primitives[primitiveIndex] = loadPrimitive(primitive, transform);
    }

    return mesh;
}

Node loadNode(cgltf_node *nodeData) {
    debug("\tNode: %s", nodeData->name);

    Node node = {
        // TODO: Initialize missing members
        .childCount = nodeData->children_count,
        .children = malloc(nodeData->children_count * sizeof(Node))
    };

    cgltf_node_transform_world(nodeData, (cgltf_float *)node.transform);

    if(nodeData->mesh) {
        node.hasMesh = 1;
        node.mesh = loadMesh(nodeData->mesh, node.transform);
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

void loadTexture(const char *path, ProtoTexture *outTexture) {
    debug("\t%s", path);
    outTexture->offset = textureBufferSize;
    outTexture->data.content = stbi_load(path, (int32_t *)&outTexture->extent.width, (int32_t *)&outTexture->extent.height, (int32_t *)&outTexture->extent.depth, STBI_rgb_alpha);
    debug("\t\tOriginal channels: %d", outTexture->extent.depth);
    outTexture->extent.depth = STBI_rgb_alpha;
    outTexture->data.size = outTexture->extent.width * outTexture->extent.height * outTexture->extent.depth;
    debug("\t\tOffset: %d", textureBufferSize);
    debug("\t\tSize: %d", outTexture->data.size);
    textureBufferSize += outTexture->data.size;
}

void loadMaterial(const char *assetsDirectory, cgltf_material *materialData, ProtoMaterial *outMaterial) {
    debug("Material Name: %s", materialData->name);
    strncpy(outMaterial->name, materialData->name, UINT8_MAX);

    if(materialData->has_pbr_metallic_roughness) {
        char textureFullPath[PATH_MAX];
        snprintf(textureFullPath, PATH_MAX, "%s/%s", assetsDirectory, materialData->pbr_metallic_roughness.base_color_texture.texture->image->uri);

        loadTexture(textureFullPath, &outMaterial->baseColor);
    }

    char normalFullPath[PATH_MAX];
    snprintf(normalFullPath, PATH_MAX, "%s/%s", assetsDirectory, materialData->normal_texture.texture->image->uri);

    loadTexture(normalFullPath, &outMaterial->normal);

    materialCount++;
}

Asset loadAsset(const char *assetName) {
    char assetsDirectory[PATH_MAX];
    makeFullPath("assets", assetsDirectory);

    char fullPath[PATH_MAX];
    snprintf(fullPath, PATH_MAX, "%s/%s", assetsDirectory, assetName);

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

    result = cgltf_load_buffers(&assetOptions, data, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    Asset asset = {
        .materialCount = data->materials_count,
        .materials = malloc(data->materials_count * sizeof(ProtoMaterial)),
        .sceneCount = data->scenes_count,
        .scenes = malloc(data->scenes_count * sizeof(Scene))
    };

    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        loadMaterial(assetsDirectory, &data->materials[materialIndex], &asset.materials[materialIndex]);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        cgltf_scene *scene = &data->scenes[sceneIndex];
        asset.scenes[sceneIndex] = loadScene(scene);
    }

    cgltf_free(data);

    return asset;
}

void movePrimitive(Primitive *primitive) {
    static uint32_t indexOffset  = 0;
    static uint32_t vertexOffset = 0;

    debug("indexOffset:  %d", indexOffset);
    debug("vertexOffset: %d", vertexOffset);

    for(uint32_t indexIndex = 0; indexIndex < primitive->indexCount; indexIndex++) {
        indexBuffer[indexOffset + indexIndex] = primitive->indices[indexIndex] + vertexOffset;
    }

    // TODO: Yeah...
    for(uint32_t vertexIndex = 0; vertexIndex < primitive->vertexCount; vertexIndex++) {
        vertexBuffer[vertexOffset + vertexIndex].position[0] = primitive->vertices[vertexIndex].position[0];
        vertexBuffer[vertexOffset + vertexIndex].position[1] = primitive->vertices[vertexIndex].position[2];
        vertexBuffer[vertexOffset + vertexIndex].position[2] = primitive->vertices[vertexIndex].position[1];

        vertexBuffer[vertexOffset + vertexIndex].texcoord[0] = primitive->vertices[vertexIndex].texcoord[0];
        vertexBuffer[vertexOffset + vertexIndex].texcoord[1] = primitive->vertices[vertexIndex].texcoord[1];
    }

    //memcpy(vertexBuffer + vertexOffset, primitive->vertices, primitive->vertexCount * sizeof(Vertex));

    indexOffset  += primitive->indexCount;
    vertexOffset += primitive->vertexCount;

    free(primitive->vertices);
    free(primitive->indices);
}

void moveMesh(Mesh *mesh) {
    debug("Copy %d primitives", mesh->primitiveCount);

    for(size_t primitiveIndex = 0; primitiveIndex < mesh->primitiveCount; primitiveIndex++) {
        movePrimitive(&mesh->primitives[primitiveIndex]);
    }

    free(mesh->primitives);
}

void moveNode(Node *node) {
    // TODO: Apply transformation

    if(node->hasMesh) {
        debug("Copy a mesh");
        moveMesh(&node->mesh);
    }

    debug("Copy %d child nodes", node->childCount);

    for(size_t childIndex = 0; childIndex < node->childCount; childIndex++) {
        moveNode(&node->children[childIndex]);
    }

    free(node->children);
}

void moveScene(Scene *scene) { // TODO: Can it be merged with node?
    debug("Copy %d nodes", scene->nodeCount);

    for(size_t nodeIndex = 0; nodeIndex < scene->nodeCount; nodeIndex++) {
        moveNode(&scene->nodes[nodeIndex]);
    }

    free(scene->nodes);
}

void moveTexture(ProtoTexture *texture) {
    memcpy(textureBuffer + texture->offset, texture->data.content, texture->data.size);

    freeData(&texture->data);
}

void moveMaterial(ProtoMaterial *material) {
    static size_t materialIndex = 0;

    protoMaterialReferences[materialIndex] = material;

    moveTexture(&material->normal);
    moveTexture(&material->baseColor);

    materialIndex++;
}

void moveAsset(Asset *asset) {
    debug("Copy %d scenes", asset->sceneCount);

    for(size_t materialIndex = 0; materialIndex < asset->materialCount; materialIndex++) {
        moveMaterial(&asset->materials[materialIndex]);
    }

    free(asset->materials);

    for(size_t sceneIndex = 0; sceneIndex < asset->sceneCount; sceneIndex++) {
        moveScene(&asset->scenes[sceneIndex]);
    }

    free(asset->scenes);
}

void createTexture(ProtoTexture *protoTexture, Image *outTexture) {
    // TODO: Check format, flags and usage
    createImage(outTexture, protoTexture->extent.width, protoTexture->extent.height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    bindImageMemory(outTexture, &deviceMemory);
    createImageView(outTexture, VK_IMAGE_ASPECT_COLOR_BIT);
    transitionImageLayout(outTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(&deviceBuffer, outTexture, protoTexture->offset);
    transitionImageLayout(outTexture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    // TODO: Free at the end
}

// TODO: Create descriptors
void createMaterial(ProtoMaterial *protoMaterial, Material *outMaterial) {
    createTexture(&protoMaterial->normal,    &outMaterial->normal   );
    createTexture(&protoMaterial->baseColor, &outMaterial->baseColor);
}

void loadAssets() {
    materialCount = 0;
    textureBufferSize = 0;

    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize  = 0;
    vertexBufferSize = 0;

    uniformSize = 0;

    assetCount = 1;
    assets = malloc(assetCount * sizeof(Asset));

    assets[0] = loadAsset("Lantern.gltf");
}

void moveAssets() {
    indexBufferSize  = indexCount  * sizeof(Index);
    vertexBufferSize = vertexCount * sizeof(Vertex);

    indexBuffer  = malloc(indexBufferSize);
    vertexBuffer = malloc(vertexBufferSize);

    textureBuffer = malloc(textureBufferSize);
    materials = malloc(materialCount * sizeof(Material));
    protoMaterialReferences = malloc(materialCount * sizeof(ProtoMaterial *));

    for(size_t assetIndex = 0; assetIndex < assetCount; assetIndex++) {
        moveAsset(&assets[assetIndex]);
    }

    free(assets);

    mempcpy(mappedSharedMemory, textureBuffer, textureBufferSize);
    copyBuffer(&sharedBuffer, &deviceBuffer, 0, 0, textureBufferSize);

    free(textureBuffer);

    for(size_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        createMaterial(protoMaterialReferences[materialIndex], &materials[materialIndex]);
    }

    free(protoMaterialReferences);

    mempcpy(mappedSharedMemory, indexBuffer, indexBufferSize);
    mempcpy(mappedSharedMemory + indexBufferSize, vertexBuffer, vertexBufferSize);
    copyBuffer(&sharedBuffer, &deviceBuffer, 0, 0, indexBufferSize + vertexBufferSize);

    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    uniformBuffer = mappedSharedMemory; // TODO: Directly write into shared memory
}
