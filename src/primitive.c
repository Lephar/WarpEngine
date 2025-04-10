#include "primitive.h"

#include "physicalDevice.h"
#include "device.h"
#include "buffer.h"
#include "pipeline.h"
#include "content.h"
#include "material.h"

#include "logger.h"

uint32_t primitiveCount;
Primitive *primitives;

void loadPrimitive(Primitive *primitive, cgltf_primitive *primitiveData, mat4 transform) {
    debug("\t\t\tPrimitive Index: %d", primitiveCount);
    debug("\t\t\tPrimitive Type:  %d", primitiveData->type);

    uint32_t materialIndex = findMaterial(primitiveData->material);
    assert(materialIndex < materialCount);

    primitive->material = &materials[materialIndex];
    debug("\t\t\t\tMaterial matched: %s", primitiveData->material->name);

    cgltf_accessor    *accessor = primitiveData->indices;
    cgltf_buffer_view *view     = accessor->buffer_view;
    cgltf_buffer      *buffer   = view->buffer;

    void *data = buffer->data + view->offset;

    primitive->indexBegin   = indexCount;
    primitive->indexCount   = accessor->count;
    primitive->vertexOffset = vertexCount;

    debug("\t\t\t\tIndices: %lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            indexBuffer[indexCount + dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        memcpy(&indexBuffer[indexCount], data, view->size);
    } // TODO: Can it be something else?

    cgltf_size primitiveVertexCount = 0;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitiveData->attributes_count; attributeIndex++) {
        cgltf_attribute *attribute = &primitiveData->attributes[attributeIndex];

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

    debug("\t\t\t\tIndex begin:   %lu", primitive->indexBegin);
    debug("\t\t\t\tIndex count:   %lu", primitive->indexCount);
    debug("\t\t\t\tVertex offset: %lu", primitive->vertexOffset);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    indexBufferSize  += accessor->count      * sizeof(Index);
    vertexBufferSize += primitiveVertexCount * sizeof(Vertex);
}
