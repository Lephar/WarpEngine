#include "content.h"

#include "physicalDevice.h"
#include "memory.h"
#include "buffer.h"
#include "image.h"
#include "light.h"
#include "camera.h"
#include "material.h"
#include "primitive.h"
#include "asset.h"
#include "control.h"
#include "descriptor.h"

#include "logger.h"
#include "numerics.h"

uint32_t indexCount;
uint32_t vertexCount;

VkDeviceSize indexBufferSize;
VkDeviceSize vertexBufferSize;

Index  *indexBuffer;
Vertex *vertexBuffer;

VkDeviceSize primitiveUniformAlignment;
VkDeviceSize cameraUniformAlignment;
VkDeviceSize materialUniformAlignment;

VkDeviceSize primitiveUniformBufferRange;
VkDeviceSize cameraUniformBufferRange;
VkDeviceSize materialUniformBufferRange;
VkDeviceSize lightUniformBufferRange;

VkDeviceSize primitiveUniformBufferOffset;
VkDeviceSize cameraUniformBufferOffset;
VkDeviceSize materialUniformBufferOffset;
VkDeviceSize lightUniformBufferOffset;

VkDeviceSize framebufferUniformBufferRange;
VkDeviceSize framebufferSetUniformBufferRange;
VkDeviceSize **framebufferUniformBufferOffsets;

void createContentBuffers() {
    const VkDeviceSize indexBufferSizeLimit  = deviceBuffer.size / 4;
    const VkDeviceSize vertexBufferSizeLimit = deviceBuffer.size - indexBufferSizeLimit;

    indexBuffer  = malloc(indexBufferSizeLimit);
    vertexBuffer = malloc(vertexBufferSizeLimit);

    nodes  = malloc(nodeCountLimit * sizeof(Node));
    scenes = malloc(nodeCountLimit * sizeof(PNode));

    cameras    = malloc(cameraCountLimit    * sizeof(Camera));
    materials  = malloc(materialCountLimit  * sizeof(Material));
    primitives = malloc(primitiveCountLimit * sizeof(Primitive));

    cameraUniforms    = malloc(cameraCountLimit    * sizeof(CameraUniform));
    primitiveUniforms = malloc(primitiveCountLimit * sizeof(PrimitiveUniform));
    materialUniforms  = malloc(materialCountLimit  * sizeof(MaterialUniform));

    pointLightUniforms       = malloc(lightCountLimit * sizeof(LightUniform));
    spotLightUniforms        = malloc(lightCountLimit * sizeof(LightUniform));
    directionalLightUniforms = malloc(lightCountLimit * sizeof(LightUniform));
    ambientLightUniforms     = malloc(lightCountLimit * sizeof(LightUniform));

    lightTypeReferences[0] = pointLightUniforms;
    lightTypeReferences[1] = spotLightUniforms;
    lightTypeReferences[2] = directionalLightUniforms;
    lightTypeReferences[3] = ambientLightUniforms;

    sceneLight = ambientLightUniforms;

    controlSets = malloc(nodeCountLimit * sizeof(ControlSet));

    indexCount  = 0;
    vertexCount = 0;

    nodeCount  = 0;
    sceneCount = 0;

    cameraCount    = 0;
    materialCount  = 0;
    primitiveCount = 0;

    controlSetCount = 0;

    debug("Content buffers created");
}

void loadContent() {
    initializeCoordinateSystem();
    debug("Coordinate system set");

    initializeLights();
    debug("Scene lighting set");

    loadAsset("assets/skybox", "Skybox.gltf");
    loadAsset("assets/main_sponza", "NewSponza_Main_glTF_003.gltf");

    debug("Assets successfully loaded");

    const uint32_t firstPersonControlSetIndex = initializeControlSet(1.0f, 10.0f, firstPersonControl);
    const uint32_t mainCameraNodeIndex = findNode("PhysCamera001");

    bindControlSet(firstPersonControlSetIndex, mainCameraNodeIndex);

    debug("Control sets successfully set");

    const VkDeviceSize storageBufferAlignment = physicalDeviceProperties.limits.minStorageBufferOffsetAlignment;

    indexBufferSize  = align(indexCount  * sizeof(Index),  storageBufferAlignment);
    vertexBufferSize = align(vertexCount * sizeof(Vertex), storageBufferAlignment);

    stagingBufferCopy(indexBuffer,  0, 0,               indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    debug("Index and vertex data copied into device memory");

    storageDescriptorSet = getStorageDescriptorSet();

    debug("Index and vertex descriptor sets created and updated");

    free(vertexBuffer);
    free(indexBuffer);

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    debug("Shared memory cleared and set for uniform buffer usage");
}

void updateSceneUniforms(PNode scene) {
    mat4 identityTransform;
    glmc_mat4_identity(identityTransform);

    for(uint32_t childIndex = 0; childIndex < scene->childCount; childIndex++) {
        updateNodeUniforms(scene->children[childIndex], identityTransform);
    }
}

void prepareUniforms() {
    for(uint32_t controlSetIndex = 0; controlSetIndex < controlSetCount; controlSetIndex++) {
        updateControlSet(&controlSets[controlSetIndex]);
    }

    for(uint32_t sceneIndex = 0; sceneIndex < sceneCount; sceneIndex++) {
        updateSceneUniforms(scenes[sceneIndex]);
    }
}

void loadUniformBuffer(uint32_t framebufferSetIndex, uint32_t framebufferIndex) {
    VkDeviceSize framebufferUniformBufferOffset = framebufferUniformBufferOffsets[framebufferSetIndex][framebufferIndex];

    for(uint32_t primitiveIndex = 0; primitiveIndex < primitiveCount; primitiveIndex++) {
        memcpy(mappedSharedMemory + framebufferUniformBufferOffset + primitiveUniformBufferOffset + primitives[primitiveIndex].uniformOffset, &primitiveUniforms[primitiveIndex], sizeof(PrimitiveUniform));
    }

    for(uint32_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++) {
        memcpy(mappedSharedMemory + framebufferUniformBufferOffset + cameraUniformBufferOffset    + cameras[cameraIndex].uniformOffset,       &cameraUniforms[cameraIndex],       sizeof(CameraUniform));
    }

    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        memcpy(mappedSharedMemory + framebufferUniformBufferOffset + materialUniformBufferOffset  + materials[materialIndex].factorOffset,    &materialUniforms[materialIndex],   sizeof(MaterialUniform));
    }

    for(uint32_t lightTypeIndex = 0; lightTypeIndex < lightTypeCount; lightTypeIndex++) {
        memcpy(mappedSharedMemory + framebufferUniformBufferOffset + lightUniformBufferOffset + lightUniformBufferRange * lightTypeIndex, lightTypeReferences[lightTypeIndex], sceneLight->iVals[lightTypeIndex] * sizeof(LightUniform));
    }
}

void freeContent() {
    for(uint32_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
        destroyNode(&nodes[nodeIndex]);
    }

    free(nodes);

    free(directionalLightUniforms);
    free(spotLightUniforms);
    free(pointLightUniforms);

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

        defaultWhiteTexture = nullptr;
    }

    if(defaultBlackTexture != nullptr) {
        destroyImageView(defaultBlackTexture);
        destroyImage(defaultBlackTexture);

        defaultBlackTexture = nullptr;
    }
}
