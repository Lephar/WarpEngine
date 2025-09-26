#include "content.h"

#include "device.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "camera.h"
#include "material.h"
#include "primitive.h"
#include "asset.h"
#include "framebuffer.h"

#include "logger.h"

uint32_t indexCount;
uint32_t vertexCount;

Index  *indexBuffer;
Vertex *vertexBuffer;

VkDeviceSize cameraUniformAlignment;
VkDeviceSize primitiveUniformAlignment;
VkDeviceSize materialUniformAlignment;

VkDeviceSize cameraUniformBufferRange;
VkDeviceSize primitiveUniformBufferRange;
VkDeviceSize materialUniformBufferRange;

VkDeviceSize framebufferUniformBufferSize;
VkDeviceSize framebufferSetUniformBufferSize;

void createContentBuffers() {
    const VkDeviceSize indexBufferSizeLimit  =     deviceBuffer.size / 4;
    const VkDeviceSize vertexBufferSizeLimit = 3 * deviceBuffer.size / 4;

    indexBuffer  = malloc(indexBufferSizeLimit);
    vertexBuffer = malloc(vertexBufferSizeLimit);

    nodes  = malloc(nodeCountLimit * sizeof(Node));
    scenes = malloc(nodeCountLimit * sizeof(uint32_t));

    cameras    = malloc(cameraCountLimit    * sizeof(Camera));
    materials  = malloc(materialCountLimit  * sizeof(Material));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    cameraUniforms    = malloc(cameraCountLimit    * sizeof(CameraUniform));
    materialUniforms  = malloc(materialCountLimit  * sizeof(MaterialUniform));
    primitiveUniforms = malloc(primitiveCountLimit * sizeof(PrimitiveUniform));

    nodeCount      = 0;
    sceneCount     = 0;

    cameraCount    = 0;
    materialCount  = 0;
    primitiveCount = 0;

    indexCount  = 0;
    vertexCount = 0;

    debug("Content buffers created");
}

void loadContent() {
    loadAsset("assets/main_sponza", "NewSponza_Main_glTF_003.gltf");

    debug("Assets successfully loaded");
    /*
    initializeWorld((vec3) {
        0.0f,
        1.0f,
        0.0f
    });

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

    debug("Scene successfully set");
    */
    const VkDeviceSize indexBufferSize  = indexCount  * sizeof(Index);
    const VkDeviceSize vertexBufferSize = vertexCount * sizeof(Vertex);

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    debug("Index and vertex data copied into device memory");

    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void updateSceneUniforms(PNode scene) {
    mat4 identityTransform;
    glmc_mat4_identity(identityTransform);

    for(uint32_t childIndex = 0; childIndex < scene->childCount; childIndex++) {
        updateNodeUniforms(&nodes[scene->childrenIndices[childIndex]], identityTransform);
    }
}

void updateUniformBuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    //updatePlayer();

    for(uint32_t sceneIndex = 0; sceneIndex < sceneCount; sceneIndex++) {
        updateSceneUniforms(&nodes[scenes[sceneIndex]]);
    }

    VkDeviceSize uniformBufferOffset = framebufferSetIndex * framebufferSetUniformBufferSize + framebufferIndex * framebufferUniformBufferSize;

    for(uint32_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        memcpy(mappedSharedMemory + uniformBufferOffset + cameraIndex * cameraUniformAlignment, &cameraUniforms[cameraIndex], sizeof(CameraUniform));
    }

    uniformBufferOffset += cameraUniformBufferRange;

    for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
        memcpy(mappedSharedMemory + uniformBufferOffset + primitiveIndex * primitiveUniformAlignment, &primitiveUniforms[primitiveIndex], sizeof(PrimitiveUniform));
    }

    uniformBufferOffset += primitiveUniformBufferRange;

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        memcpy(mappedSharedMemory + uniformBufferOffset + materialIndex * materialUniformAlignment, &materialUniforms[materialIndex], sizeof(MaterialUniform));
    }
}

void bindContentBuffers(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    VkVertexInputBindingDescription2EXT vertexBinding = {
        .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT,
        .pNext = nullptr,
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
        .divisor = 1
    };

    VkVertexInputAttributeDescription2EXT vertexAttributes[] = {
        {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = nullptr,
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, position)
        },{
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = nullptr,
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
            .offset = offsetof(Vertex, tangent)
        },{
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = nullptr,
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = offsetof(Vertex, normal)
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = nullptr,
            .location = 3,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texcoord0)
        }, {
            .sType = VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT,
            .pNext = nullptr,
            .location = 4,
            .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = offsetof(Vertex, texcoord1)
        }
    };

    uint32_t vertexAttributeCount = sizeof(vertexAttributes) / sizeof(VkVertexInputAttributeDescription2EXT);

    const VkDeviceSize vertexBufferOffset = indexCount * sizeof(Index);

    vkCmdBindIndexBuffer(framebuffer->renderCommandBuffer, deviceBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindVertexBuffers(framebuffer->renderCommandBuffer, 0, 1, &deviceBuffer.buffer, &vertexBufferOffset);

    PFN_vkCmdSetVertexInputEXT cmdSetVertexInput = loadDeviceFunction("vkCmdSetVertexInputEXT");
    cmdSetVertexInput(framebuffer->renderCommandBuffer, 1, &vertexBinding, vertexAttributeCount, vertexAttributes);
}

void freeContent() {
    for(uint32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
        destroyNode(&nodes[nodeIndex]);
    }

    free(nodes);

    free(cameraUniforms);
    free(cameras);

    free(primitiveUniforms);
    free(primitives);

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        destroyMaterial(&materials[materialIndex]);
    }

    free(materialUniforms);
    free(materials);

    if(defaultWhiteTexture != nullptr) {
        destroyImageView(defaultWhiteTexture);
        destroyImage(defaultWhiteTexture);
    }

    if(defaultBlackTexture != nullptr) {
        destroyImageView(defaultBlackTexture);
        destroyImage(defaultBlackTexture);
    }
}
