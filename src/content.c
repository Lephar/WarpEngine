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

extern VkDescriptorSetLayout descriptorSetLayout;
extern VkDescriptorPool descriptorPool;

uint64_t indexCount;
uint64_t vertexCount;
uint64_t indexBufferSize;
uint64_t vertexBufferSize;

const uint64_t indexBufferSizeLimit   = 1L << 30;
const uint64_t vertexBufferSizeLimit  = 1L << 30;

Index *indexBuffer;
Vertex *vertexBuffer;
Uniform *uniformBuffer;

uint32_t materialCount;
uint32_t drawableCount;

const uint32_t materialCountLimit = 128;
const uint32_t drawableCountLimit = 128;

Material *materials;
Drawable *drawables;

void loadPrimitive(cgltf_primitive *primitive, mat4 transform) {
    assert(drawableCount < drawableCountLimit);

    Drawable *drawable = &drawables[drawableCount];

    debug("\t\t\tDrawable Count: %d", drawableCount);
    debug("\t\t\tPrimitive Type: %d", primitive->type);

    for(cgltf_size materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(primitive->material->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            drawable->descriptorReference = &materials[materialIndex].descriptor;
            debug("\t\t\t\tMaterial matched: %s", primitive->material->name);
            break;
        }
    }

    assert(drawable->descriptorReference != NULL);

    cgltf_accessor *accessor = primitive->indices;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    void *data = buffer->data + view->offset;

    drawable->indexBegin   = indexCount;
    drawable->indexCount   = accessor->count;
    drawable->vertexOffset = vertexCount;

    debug("\t\t\t\tIndices: %lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            indexBuffer[indexCount + dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        memcpy(&indexBuffer[indexCount], data, view->size);
    } // TODO: Can it be something else?

    cgltf_size primitiveVertexCount = 0;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitive->attributes_count; attributeIndex++) {
        cgltf_attribute *attribute = &primitive->attributes[attributeIndex];

        cgltf_accessor *attributeAccessor = attribute->data;
        cgltf_buffer_view *attributeView = attributeAccessor->buffer_view;
        cgltf_buffer *attributeBuffer = attributeView->buffer;

        void *attributeData = attributeBuffer->data + attributeView->offset;

        if(primitiveVertexCount == 0) {
            primitiveVertexCount = attributeAccessor->count;
        }

        assert(primitiveVertexCount == attributeAccessor->count);

        debug("\t\t\t\tAttribute %s: %lu elements of type %lu, total of %lu bytes in size", attribute->name, attributeAccessor->count, attributeAccessor->type, attributeView->size);

        // TODO: Check component data types too
        if(attribute->type == cgltf_attribute_type_position) {
            vec3 *positions = attributeData;

            for(cgltf_size positionIndex = 0; positionIndex < attributeAccessor->count; positionIndex++) {
                vec3 *position = &vertexBuffer[vertexCount + positionIndex].position;

                glm_mat4_mulv3(transform, positions[positionIndex], 1.0f, *position);
                (*position)[1] *= -1;
                /*
                float scalar   = (*position)[1];
                (*position)[1] = (*position)[2];
                (*position)[2] = scalar;
                */
            }
        } else if(attribute->type == cgltf_attribute_type_texcoord) {
            vec2 *texcoords = attributeData;

            for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                memcpy(vertexBuffer[vertexCount + texcoordIndex].texcoord, texcoords[texcoordIndex], sizeof(vec2));
            }
        } //TODO: Load normal and tangent too
    }

    debug("\t\t\t\tIndex begin:   %lu", drawable->indexBegin);
    debug("\t\t\t\tIndex count:   %lu", drawable->indexCount);
    debug("\t\t\t\tVertex offset: %lu", drawable->vertexOffset);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    indexBufferSize  += accessor->count      * sizeof(Index);
    vertexBufferSize += primitiveVertexCount * sizeof(Vertex);

    drawableCount++;
}

void loadMesh(cgltf_mesh *mesh, mat4 transform) {
    debug("\t\tMesh: %s", mesh->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
        loadPrimitive(&mesh->primitives[primitiveIndex], transform);
    }
}

void loadNode(cgltf_node *node) {
    debug("\tNode: %s", node->name);

    mat4 transform;
    cgltf_node_transform_world(node, (cgltf_float *)transform);

    if(node->mesh) {
        loadMesh(node->mesh, transform);
    }

    for(cgltf_size childIndex = 0; childIndex < node->children_count; childIndex++) {
        loadNode(node->children[childIndex]);
    }
}

void loadScene(cgltf_scene *scene) {
    debug("Scene: %s", scene->name);

    for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
        loadNode(scene->nodes[nodeIndex]);
    }
}

void loadTexture(const char *path, Image *outTexture) {
    debug("\tImage Path: %s", path);
    void *imageData = stbi_load(path, (int32_t *)&outTexture->extent.width, (int32_t *)&outTexture->extent.height, (int32_t *)&outTexture->extent.depth, STBI_rgb_alpha);
    outTexture->extent.depth = STBI_rgb_alpha;
    VkDeviceSize imageSize = outTexture->extent.width * outTexture->extent.height * outTexture->extent.depth;

    uint32_t maxDimension = outTexture->extent.width < outTexture->extent.height ? outTexture->extent.height : outTexture->extent.width;
    uint32_t mips = (uint32_t) floor(log2(maxDimension)) + 1;

    debug("\t\tImage Size:    %lu", imageSize);
    debug("\t\tMemory Offset: %lu", deviceMemory.offset);
    debug("\t\tMip Levels:    %u", mips);

    createImage(outTexture, outTexture->extent.width, outTexture->extent.height, mips, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
    bindImageMemory(outTexture, &deviceMemory);
    createImageView(outTexture);
    transitionImageLayout(outTexture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    if(imageSize <= sharedBuffer.size) {
        debug("\t\tCopying image data using shared buffer");
        memcpy(mappedSharedMemory, imageData, imageSize);
        copyBufferToImage(&sharedBuffer, outTexture, 0);
    } else if (imageSize <= deviceBuffer.size) {
        debug("\t\tCopying image data using staging device local buffer");
        stagingBufferCopy(imageData, 0, 0, imageSize);
        copyBufferToImage(&deviceBuffer, outTexture, 0);
    } else {
        debug("Can't copy image data, increase shared or device local buffer size!");
        assert(imageSize <= sharedBuffer.size || imageSize <= deviceBuffer.size);
    }

    generateMipmaps(outTexture);

    stbi_image_free(imageData);
    debug("\t\tImage created");
}

void createDescriptor(Material *material) {
    VkSamplerCreateInfo samplerInfo = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_TRUE,
        .maxAnisotropy = 16.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.0f,
        .maxLod = (float) material->baseColor.mips,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_WHITE,
        .unnormalizedCoordinates = VK_FALSE
    };

    vkCreateSampler(device, &samplerInfo, NULL, &material->sampler);
    debug("\t\tImage sampler created");

    VkDescriptorSetAllocateInfo allocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };

    vkAllocateDescriptorSets(device, &allocateInfo, &material->descriptor);

    VkDescriptorBufferInfo bufferInfo = {
        .buffer = sharedBuffer.buffer,
        .offset = 0,
        .range = sizeof(Uniform)
    };

    VkDescriptorImageInfo imageInfo = {
        .sampler = material->sampler,
        .imageView = material->baseColor.view,
        .imageLayout = material->baseColor.layout
    };

    VkWriteDescriptorSet descriptorWrites[] = {
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = material->descriptor,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = NULL
        }, {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = NULL,
            .dstSet = material->descriptor,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = NULL,
            .pTexelBufferView = NULL
        }
    };

    vkUpdateDescriptorSets(device, sizeof(descriptorWrites) / sizeof(VkWriteDescriptorSet), descriptorWrites, 0, NULL);
    debug("\t\tDescriptor created");
}

void loadMaterial(cgltf_material *materialData) {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(materialData->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            return;
        }
    }

    assert(materialCount < materialCountLimit);
    Material *material = &materials[materialCount];

    debug("Material Name: %s", materialData->name);
    strncpy(material->name, materialData->name, UINT8_MAX);

    if(materialData->has_pbr_metallic_roughness) {
        char textureFullPath[PATH_MAX];
        makeFullPath(materialData->pbr_metallic_roughness.base_color_texture.texture->image->uri, "assets", textureFullPath);

        loadTexture(textureFullPath, &material->baseColor);
        createDescriptor(material);
    }

    materialCount++;
}

void loadAsset(const char *assetName) {
    char fullPath[PATH_MAX];
    makeFullPath(assetName, "assets", fullPath);

    cgltf_data *data = NULL;
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

    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        loadMaterial(&data->materials[materialIndex]);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        loadScene(&data->scenes[sceneIndex]);
    }

    cgltf_free(data);
}

void loadAssets() {
    indexCount  = 0;
    vertexCount = 0;

    indexBufferSize   = 0;
    vertexBufferSize  = 0;

    indexBuffer   = malloc(indexBufferSizeLimit);
    vertexBuffer  = malloc(vertexBufferSizeLimit);

    assert(indexBuffer);
    assert(vertexBuffer);

    drawableCount = 0;
    materialCount = 0;

    materials = malloc(materialCountLimit * sizeof(Material));
    drawables = malloc(drawableCountLimit * sizeof(Drawable));

    //loadAsset("Lantern.gltf");
    loadAsset("Scene.gltf");
    debug("Assets successfully loaded");

    stagingBufferCopy(indexBuffer,  0, 0, indexBufferSize);
    stagingBufferCopy(vertexBuffer, 0, indexBufferSize, vertexBufferSize);

    free(vertexBuffer);
    free(indexBuffer);
    debug("Index and vertex data copied into device memory");

    memset(mappedSharedMemory, 0, sharedBuffer.size);

    uniformBuffer = mappedSharedMemory;
}

void freeAssets() {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        vkDestroySampler(device, materials[materialIndex].sampler, NULL);
        destroyImageView(&materials[materialIndex].baseColor);
        destroyImage(&materials[materialIndex].baseColor);
    }

    free(materials);
    free(drawables);

    debug("Assets freed");
}