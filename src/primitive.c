#include "primitive.h"

#include "content.h"
#include "material.h"

#include "file.h"
#include "logger.h"

const uint32_t primitiveCountLimit = 128;
uint32_t primitiveCount;
Primitive *primitives;

void loadPrimitive(Primitive *primitiveReference, cgltf_primitive *primitive, mat4 transform) {
    debug("\t\t\tPrimitive Index: %d", primitiveCount);
    debug("\t\t\tPrimitive Type:  %d", primitive->type);

    primitiveReference->material = NULL;

    for(cgltf_size materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(primitive->material->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            primitiveReference->material = &materials[materialIndex];
            debug("\t\t\t\tMaterial matched: %s", primitive->material->name);
            break;
        }
    }

    assert(primitiveReference->material != NULL);

    cgltf_accessor *accessor = primitive->indices;
    cgltf_buffer_view *view = accessor->buffer_view;
    cgltf_buffer *buffer = view->buffer;

    void *data = buffer->data + view->offset;

    primitiveReference->indexBegin   = indexCount;
    primitiveReference->indexCount   = accessor->count;
    primitiveReference->vertexOffset = vertexCount;

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

                glmc_mat4_mulv3(transform, positions[positionIndex], 1.0f, *position);

                /*// NOTICE: Use this if glTF model is exported with +Y up
                float scalar   = (*position)[1];
                (*position)[1] = (*position)[2];
                (*position)[2] = scalar;*/
            }
        } else if(attribute->type == cgltf_attribute_type_texcoord) {
            vec2 *texcoords = attributeData;

            for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                memcpy(vertexBuffer[vertexCount + texcoordIndex].texcoord, texcoords[texcoordIndex], sizeof(vec2));
            }
        } // TODO: Load normal and tangent too
    }

    debug("\t\t\t\tIndex begin:   %lu", primitiveReference->indexBegin);
    debug("\t\t\t\tIndex count:   %lu", primitiveReference->indexCount);
    debug("\t\t\t\tVertex offset: %lu", primitiveReference->vertexOffset);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    indexBufferSize  += accessor->count      * sizeof(Index);
    vertexBufferSize += primitiveVertexCount * sizeof(Vertex);
}

void loadMesh(cgltf_mesh *mesh, mat4 transform) {
    debug("\t\tMesh: %s", mesh->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
        assert(primitiveCount < primitiveCountLimit);

        Primitive *primitiveReference = &primitives[primitiveCount];

        loadPrimitive(primitiveReference, &mesh->primitives[primitiveIndex], transform);

        primitiveCount++;
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

cgltf_data *loadAsset(const char *assetName) {
    char fullPath[PATH_MAX];
    makeFullPath("data", assetName, fullPath);

    cgltf_data *data = NULL;
    cgltf_options assetOptions = {};
    cgltf_result result;

    result = cgltf_parse_file(&assetOptions, fullPath, &data);

    if(result != cgltf_result_success) {
        debug("Failed to read %s: %d", assetName, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_validate(data);

    if(result != cgltf_result_success) {
        debug("Failed to validate %s: %d", assetName, result);
        assert(result == cgltf_result_success);
    }

    result = cgltf_load_buffers(&assetOptions, data, fullPath);

    if(result != cgltf_result_success) {
        debug("Failed to load buffers %s: %d", assetName, result);
        cgltf_free(data);
        assert(result == cgltf_result_success);
    }

    return data;
}

void processAsset(cgltf_data *data) {
    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        loadMaterial(&data->materials[materialIndex]);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        loadScene(&data->scenes[sceneIndex]);
    }
}

void freeAsset(cgltf_data *data) {
    cgltf_free(data);
}
