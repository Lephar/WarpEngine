#include "asset.h"

#include "material.h"
#include "primitive.h"

#include "file.h"
#include "logger.h"

uint32_t nodeCountLimit;
uint32_t nodeCount;
PNode nodes;

uint32_t sceneCount;
uint32_t *scenes;

void initializeNode(PNode node) {
    node->parentIndex = UINT32_MAX;
    node->meshCount = 0;
    node->meshIndices = nullptr;
    node->childCount = 0;
    node->childrenIndices = nullptr;
    glmc_mat4_identity(node->scale);
    glmc_mat4_identity(node->rotation);
    glmc_mat4_identity(node->translation);
}

uint32_t loadNode(uint32_t parentIndex, cgltf_node *nodeData) {
    debug("Node: %s", nodeData->name);

    assert(nodeCount < nodeCountLimit);
    uint32_t nodeIndex = nodeCount;
    nodeCount++;

    PNode node = &nodes[nodeIndex];
    initializeNode(node);

    node->parentIndex = parentIndex;
    node->childCount = nodeData->children_count;
    node->childrenIndices = malloc(node->childCount * sizeof(uint32_t)); // NOTICE: malloc(0) is completely safe

    debug("\tChild Count: %u", node->childCount);

    for(uint32_t childIndex = 0; childIndex < node->childCount; childIndex++) {
        cgltf_node *childData = nodeData->children[childIndex];
        node->childrenIndices[childIndex] = loadNode(childIndex, childData);
    }

    if(nodeData->mesh) {
        cgltf_mesh *meshData = nodeData->mesh;

        node->meshCount = meshData->primitives_count;
        node->meshIndices = malloc(node->meshCount * sizeof(uint32_t));

        debug("Mesh Count: %u", node->meshCount);

        for(cgltf_size primitiveIndex = 0; primitiveIndex < node->meshCount; primitiveIndex++) {
            cgltf_primitive *primitive = &meshData->primitives[primitiveIndex];
            node->meshIndices[primitiveIndex] = loadPrimitive(primitive);
        }
    }

    return nodeIndex;
}

uint32_t loadScene(cgltf_scene *sceneData) {
    assert(nodeCount < nodeCountLimit);
    uint32_t nodeIndex = nodeCount;
    nodeCount++;

    PNode scene = &nodes[nodeIndex];
    initializeNode(scene);

    scene->childCount = sceneData->nodes_count;
    scene->childrenIndices = malloc(scene->childCount * sizeof(uint32_t));

    debug("Node Count: %u", scene->childCount);

    for(uint32_t childIndex = 0; childIndex < scene->childCount; childIndex++) {
        cgltf_node *childData = sceneData->nodes[childIndex];
        scene->childrenIndices[childIndex] = loadNode(nodeIndex, childData);
    }

    return nodeIndex;
}

void loadAsset(const char *subdirectory, const char *filename) {
    char fullPath[PATH_MAX];
    makeFullPath(subdirectory, filename, fullPath);

    cgltf_data *assetData = nullptr;
    cgltf_options assetOptions = {};
    cgltf_result result = cgltf_parse_file(&assetOptions, fullPath, &assetData);

    if(result != cgltf_result_success) {
        debug("Failed to read %s: %d", filename, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(assetData);

    if(result != cgltf_result_success) {
        debug("Failed to validate %s: %d", filename, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_load_buffers(&assetOptions, assetData, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", filename, result);
        cgltf_free(assetData);
        assert(result == cgltf_result_success);
    }

    for(cgltf_size materialIndex = 0; materialIndex < assetData->materials_count; materialIndex++) {
        cgltf_material *materialData = &assetData->materials[materialIndex];

        if(findMaterial(materialData) < materialCount) {
            debug("Material already found, skipping...");
            continue;
        }

        loadMaterial(subdirectory, materialData);
    }

    assert(sceneCount < nodeCountLimit);
    uint32_t sceneIndex = sceneCount;
    sceneCount++;

    scenes[sceneIndex] = loadScene(assetData->scene);

    cgltf_free(assetData);
}

void drawNode(uint32_t nodeIndex) {

}

void drawScene(uint32_t sceneIndex) {

}