#include "asset.h"

#include "light.h"
#include "camera.h"
#include "control.h"
#include "material.h"
#include "primitive.h"

#include "file.h"
#include "logger.h"

uint32_t nodeCountLimit;
uint32_t nodeCount;
PNode nodes;

uint32_t sceneCount;
PNode *scenes;

void initializeNode(PNode node) {
    memset(node->name, 0, UINT8_MAX);
    node->controlSet = nullptr;
    node->lightIndex = UINT32_MAX;
    node->cameraIndex = UINT32_MAX;
    node->meshCount = 0;
    node->meshIndices = nullptr;
    node->childCount = 0;
    node->children = nullptr;
    glmc_mat4_identity(node->transform);
}

uint32_t findNode(const char *name) {
    for(uint32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
        if(strncmp(name, nodes[nodeIndex].name, UINT8_MAX) == 0) {
            return nodeIndex;
        }
    }

    return UINT32_MAX;
}

uint32_t loadNode(cgltf_node *nodeData) {
    assert(nodeCount < nodeCountLimit);
    const uint32_t nodeIndex = nodeCount;
    nodeCount++;

    PNode node = &nodes[nodeIndex];
    initializeNode(node);

    debug("Node Name: %s", nodeData->name);
    strncpy(node->name, nodeData->name, UINT8_MAX);

    cgltf_node_transform_local(nodeData, (cgltf_float *) node->transform);

    if(nodeData->light) {
        cgltf_light *lightData = nodeData->light;
        node->lightIndex = loadLight(lightData);
    }

    if(nodeData->camera) {
        cgltf_camera *cameraData = nodeData->camera;
        node->cameraIndex = loadCamera(cameraData);
    }

    if(nodeData->mesh) {
        cgltf_mesh *meshData = nodeData->mesh;

        debug("\tMesh Name: %s", meshData->name);
        debug("\t\tPrimitive Count: %d", meshData->primitives_count);

        node->meshCount = meshData->primitives_count;
        node->meshIndices = malloc(node->meshCount * sizeof(uint32_t));

        for(cgltf_size primitiveIndex = 0; primitiveIndex < node->meshCount; primitiveIndex++) {
            cgltf_primitive *primitive = &meshData->primitives[primitiveIndex];
            node->meshIndices[primitiveIndex] = loadPrimitive(primitive);
        }
    }

    if(nodeData->children_count != 0) {
        debug("\tChild Node Count: %d", nodeData->children_count);

        node->childCount = nodeData->children_count;
        node->children = malloc(node->childCount * sizeof(PNode));

        for(uint32_t childIndex = 0; childIndex < node->childCount; childIndex++) {
            cgltf_node *childData = nodeData->children[childIndex];
            uint32_t childNodeIndex = loadNode(childData);
            node->children[childIndex] = &nodes[childNodeIndex];
        }
    }

    return nodeIndex;
}

uint32_t loadScene(cgltf_scene *sceneData) {
    assert(nodeCount < nodeCountLimit);
    const uint32_t nodeIndex = nodeCount;
    nodeCount++;

    PNode scene = &nodes[nodeIndex];
    initializeNode(scene);

    scene->childCount = sceneData->nodes_count;
    scene->children = malloc(scene->childCount * sizeof(PNode));

    for(uint32_t childIndex = 0; childIndex < scene->childCount; childIndex++) {
        cgltf_node *childData = sceneData->nodes[childIndex];
        uint32_t childNodeIndex = loadNode(childData);
        scene->children[childIndex] = &nodes[childNodeIndex];
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

        if(findMaterial(materialData->name) < materialCount) {
            debug("Material already found, skipping...");
            continue;
        }

        loadMaterial(subdirectory, materialData);
    }

    assert(sceneCount < nodeCountLimit);
    const uint32_t sceneIndex = sceneCount;
    sceneCount++;

    uint32_t nodeIndex = loadScene(assetData->scene);
    scenes[sceneIndex] = &nodes[nodeIndex];

    cgltf_free(assetData);
}

void updateNodeUniforms(PNode node, mat4 transform) {
    mat4 nodeTransform;
    glmc_mat4_identity(nodeTransform);

    if(node->controlSet != nullptr) {
        glmc_mul(node->controlSet->transform, nodeTransform, nodeTransform);
    }

    glmc_mul(node->transform, nodeTransform, nodeTransform);
    glmc_mul(transform, nodeTransform, nodeTransform);

    if(node->lightIndex != UINT32_MAX) {
        PLightUniform lightUniform = &pointLightUniforms[node->lightIndex];

        glmc_mat4_copy(nodeTransform, lightUniform->transform);
    }

    if(node->cameraIndex != UINT32_MAX) {
        PCameraUniform cameraUniform = &cameraUniforms[node->cameraIndex];

        glmc_mat4_copy(nodeTransform, cameraUniform->transform);
        glmc_mat4_inv(cameraUniform->transform, cameraUniform->view);
        glmc_mat4_mul(cameraUniform->projection, cameraUniform->view, cameraUniform->projectionView);
    }

    for(uint32_t meshIndex = 0; meshIndex < node->meshCount; meshIndex++) {
        glmc_mat4_copy(nodeTransform, primitiveUniforms[node->meshIndices[meshIndex]].model);
        glmc_mat4_inv(primitiveUniforms[node->meshIndices[meshIndex]].model, primitiveUniforms[node->meshIndices[meshIndex]].normal);
        glmc_mat4_transpose(primitiveUniforms[node->meshIndices[meshIndex]].normal);
    }

    for(uint32_t childIndex = 0; childIndex < node->childCount; childIndex++) {
        updateNodeUniforms(node->children[childIndex], nodeTransform);
    }
}

void destroyNode(PNode node) {
    if(node->childCount > 0) {
        free(node->children);
        node->childCount = 0;
    }

    if(node->meshCount > 0) {
        free(node->meshIndices);
        node->meshCount = 0;
    }
}