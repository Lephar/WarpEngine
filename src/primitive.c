#include "primitive.h"

#include "content.h"
#include "material.h"

#include "file.h"
#include "logger.h"

const uint32_t primitiveCountLimit = 128;
uint32_t primitiveCount;
Primitive *primitives;

void loadPrimitive(ContentType type, cgltf_primitive *primitive, mat4 transform) {
    assert(primitiveCount < primitiveCountLimit);

    Primitive *primitiveReference = &primitives[primitiveCount];
     primitiveReference->type = malloc(sizeof(ContentType));
    *primitiveReference->type = type;

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

                glm_mat4_mulv3(transform, positions[positionIndex], 1.0f, *position);

                // NOTICE: Use this if glTF model is exported with +Z up
                // TODO: Temporary fix until --finvert-y flag for glslc is fixed
                (*position)[1] *= -1;

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

    primitiveCount++;
}

void loadMesh(ContentType type, cgltf_mesh *mesh, mat4 transform) {
    debug("\t\tMesh: %s", mesh->name);

    for(cgltf_size primitiveIndex = 0; primitiveIndex < mesh->primitives_count; primitiveIndex++) {
        loadPrimitive(type, &mesh->primitives[primitiveIndex], transform);
    }
}

void loadNode(ContentType type, cgltf_node *node) {
    debug("\tNode: %s", node->name);

    mat4 transform;
    cgltf_node_transform_world(node, (cgltf_float *)transform);

    if(node->mesh) {
        loadMesh(type, node->mesh, transform);
    }

    for(cgltf_size childIndex = 0; childIndex < node->children_count; childIndex++) {
        loadNode(type, node->children[childIndex]);
    }
}

void loadScene(ContentType type, cgltf_scene *scene) {
    debug("Scene: %s", scene->name);

    for (cgltf_size nodeIndex = 0; nodeIndex < scene->nodes_count; nodeIndex++) {
        loadNode(type, scene->nodes[nodeIndex]);
    }
}

void loadAsset(ContentType type, const char *assetName) {
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

    for(cgltf_size materialIndex = 0; materialIndex < data->materials_count; materialIndex++) {
        loadMaterial(&data->materials[materialIndex]);
    }

    for(cgltf_size sceneIndex = 0; sceneIndex < data->scenes_count; sceneIndex++) {
        loadScene(type, &data->scenes[sceneIndex]);
    }

    cgltf_free(data);
}
