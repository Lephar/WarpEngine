#include "content.h"

#include "device.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "meta.h"
#include "primitive.h"

#include "file.h"
#include "logger.h"

uint32_t indexCount;
uint32_t vertexCount;

VkDeviceSize indexBufferSize;
VkDeviceSize vertexBufferSize;
VkDeviceSize uniformBufferSize;

VkDeviceSize sceneUniformAlignment;
VkDeviceSize primitiveUniformAlignment;
VkDeviceSize dynamicUniformBufferRange;
VkDeviceSize framebufferUniformStride;

uint32_t primitiveCountLimit;

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

        assert(materialCount < primitiveCountLimit);
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

    const VkDeviceSize indexBufferSizeLimit   = deviceMemory.size / 2;
    const VkDeviceSize vertexBufferSizeLimit  = deviceMemory.size / 2;
    const VkDeviceSize uniformBufferSizeLimit = sharedMemory.size;

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);
    uniformBuffer = malloc(uniformBufferSizeLimit);

    assert(indexBuffer);
    assert(vertexBuffer);
    assert(uniformBuffer);

    materialCount  = 0;
    primitiveCount = 0;

    materials  = malloc(primitiveCountLimit * sizeof(Material));
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

    free(uniformBuffer);
    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void bindContentBuffers(VkCommandBuffer commandBuffer) {
    VkVertexInputBindingDescription2EXT vertexBinding = {
        .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .pNext = NULL,
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor = 1
    };

    VkVertexInputAttributeDescription2EXT vertexAttributes[] = {
        {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, position)
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texcoord)
        }
    };

    uint32_t vertexAttributeCount = sizeof(vertexAttributes) / sizeof(VkVertexInputAttributeDescription2EXT);

    vkCmdBindIndexBuffer(commandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &deviceBuffer.buffer, &indexBufferSize);

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadDeviceFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(commandBuffer, 1, &vertexBinding, vertexAttributeCount, vertexAttributes);
}

void freeContent() {
    free(primitives);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        destroyMaterial(&materials[materialIndex]);
    }

    free(materials);
}
