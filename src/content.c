#include "content.h"

#include "device.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "material.h"
#include "scene.h"
#include "primitive.h"
#include "descriptor.h"

#include "file.h"
#include "logger.h"

uint32_t indexCount;
uint32_t vertexCount;

Index  *indexBuffer;
Vertex *vertexBuffer;

VkDeviceSize sceneUniformAlignment;
VkDeviceSize primitiveUniformAlignment;
VkDeviceSize materialUniformAlignment;

VkDeviceSize primitiveUniformBufferRange;
VkDeviceSize materialUniformBufferRange;

VkDeviceSize framebufferSetUniformBufferOffset;
VkDeviceSize framebufferUniformBufferStride;

void loadAsset(const char *subdirectory, const char *filename) {
    char fullPath[PATH_MAX];
    makeFullPath(subdirectory, filename, fullPath);

    cgltf_data *data = NULL;
    cgltf_options assetOptions = {};
    cgltf_result result;

    result = cgltf_parse_file(&assetOptions, fullPath, &data);

    if(result != cgltf_result_success) {
        debug("Failed to read %s: %d", filename, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(data);

    if(result != cgltf_result_success) {
        debug("Failed to validate %s: %d", filename, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_load_buffers(&assetOptions, data, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", filename, result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        cgltf_material *materialData = &data->materials[materialIndex];

        if(findMaterial(materialData) < materialCount) {
            debug("Material already found, skipping...");
            continue;
        }

        loadMaterial(subdirectory, materialData);
    }

    for(cgltf_size nodeIndex = 0; nodeIndex < data->nodes_count; nodeIndex++) {
        cgltf_node *nodeData = &data->nodes[nodeIndex];

        if(nodeData->mesh) {
            mat4 transform;
            cgltf_node_transform_world(nodeData, (cgltf_float *) transform);

            cgltf_mesh *meshData = nodeData->mesh;

            for(cgltf_size primitiveIndex = 0; primitiveIndex < meshData->primitives_count; primitiveIndex++) {
                loadPrimitive(&meshData->primitives[primitiveIndex], transform);
            }
        }
    }

    cgltf_free(data);
}

void createContentBuffers() {
    const VkDeviceSize indexBufferSizeLimit  =     deviceBuffer.size / 4;
    const VkDeviceSize vertexBufferSizeLimit = 3 * deviceBuffer.size / 4;

    indexBuffer  = malloc(indexBufferSizeLimit);
    vertexBuffer = malloc(vertexBufferSizeLimit);

    materials  = malloc(primitiveCountLimit * sizeof(Material));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    materialUniforms  = malloc(primitiveCountLimit * sizeof(MaterialUniform));
    primitiveUniforms = malloc(primitiveCountLimit * sizeof(PrimitiveUniform));

    materialCount  = 0;
    primitiveCount = 0;

    indexCount  = 0;
    vertexCount = 0;

    debug("Content buffers created");
}

void loadContent() {
    loadAsset("assets/main_sponza", "NewSponza_Main_glTF_003.gltf");

    debug("Assets successfully loaded");

    initializeScene();

    initializePlayer((vec3) {
        0.0f,
        2.0f,
        0.0f
    },
    (vec3) {
        1.0f,
        0.0f,
        0.0f
    },
    10.0f);

    initializeCamera(M_PI_4, 1.0f, 128.0f);

    debug("Scene successfully set");

    const VkDeviceSize indexBufferSize  = indexCount  * sizeof(Index);
    const VkDeviceSize vertexBufferSize = vertexCount * sizeof(Vertex);

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    debug("Index and vertex data copied into device memory");

    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");

    factorDescriptorSet = getFactorDescriptorSet();

    debug("Material factors copied to uniform buffer and descriptor set created");
}

void updateUniforms(uint32_t framebufferIndex) {
    updatePlayer();
    updateCamera();

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        memcpy(mappedSharedMemory + materialIndex * materialUniformAlignment, &materialUniforms[materialIndex], sizeof(MaterialUniform));
    }

    VkDeviceSize framebufferUniformBufferOffset = framebufferSetUniformBufferOffset + framebufferIndex * framebufferUniformBufferStride;

    for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
        memcpy(mappedSharedMemory + framebufferUniformBufferOffset + primitiveIndex * primitiveUniformAlignment, &primitiveUniforms[primitiveIndex], sizeof(PrimitiveUniform));
    }

    memcpy(mappedSharedMemory + framebufferUniformBufferOffset + primitiveUniformBufferRange, &sceneUniform, sceneUniformAlignment);
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
        },{
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(Vertex, tangent)
        },{
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, normal)
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texcoord0)
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = NULL,
            .location = 4,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texcoord1)
        }
    };

    uint32_t vertexAttributeCount = sizeof(vertexAttributes) / sizeof(VkVertexInputAttributeDescription2EXT);

    const VkDeviceSize vertexBufferOffset = indexCount * sizeof(Index);

    vkCmdBindIndexBuffer(commandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &deviceBuffer.buffer, &vertexBufferOffset);

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
