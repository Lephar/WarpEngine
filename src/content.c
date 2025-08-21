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

SceneUniform *sceneUniform;

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
            continue;
        }

        assert(materialCount < primitiveCountLimit);
        Material *material = &materials[materialCount];

        loadMaterial(subdirectory, material, materialData);
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
            }
        }
    }

    cgltf_free(data);
}

void createContentBuffers() {
    const VkDeviceSize indexBufferSizeLimit   =     deviceBuffer.size / 4;
    const VkDeviceSize vertexBufferSizeLimit  = 3 * deviceBuffer.size / 4;
    const VkDeviceSize uniformBufferSizeLimit = framebufferUniformStride;

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);
    uniformBuffer = calloc(uniformBufferSizeLimit, 1);

    materials  = malloc(primitiveCountLimit * sizeof(Material));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;
    uniformBufferSize = sceneUniformAlignment;

    sceneUniform = uniformBuffer;

    materialCount  = 0;
    primitiveCount = 0;

    debug("Content buffers created");
}

void loadContent() {
    loadAsset("assets/main_sponza", "NewSponza_Main_glTF_003.gltf");

    debug("Assets successfully loaded");

    initializeScene();

    initializePlayer((vec3) {
        0.0f,
        0.0f,
        8.0f
    },
    (vec3) {
        1.0f,
        1.0f,
        0.0f
    },
    10.0f);

    initializeCamera(M_PI_4, 1.0f, 1000.0f);

    initializeActor((vec3) {
        8.0f,
        8.0f,
        -8.0f
    },
    (vec3) {
        1.0f,
        1.0f,
        0.0f
    },
    10.0f);

    debug("Meta materials successfully set");

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    debug("Index and vertex data copied into device memory");

    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void updateUniforms(uint32_t framebufferIndex) {
    //updateSkybox();
    updatePlayer();
    //updateActor();
    updateCamera();

    memcpy(mappedSharedMemory + framebufferIndex * framebufferUniformStride, uniformBuffer, framebufferUniformStride);
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

    free(uniformBuffer);
}
