#include "primitive.h"

#include "pipeline.h"
#include "content.h"
#include "material.h"

#include "logger.h"

uint32_t primitiveCount;
Primitive *primitives;

void loadPrimitive(Primitive *primitive, cgltf_primitive *primitiveData, mat4 transform) {
    debug("Primitive Index: %d", primitiveCount);
    debug("\tType: %d", primitiveData->type);

    uint32_t materialIndex = findMaterial(primitiveData->material);

    if(materialIndex >= materialCount) {
        debug("\tNo material found for the primitive, skipping...");
        return;
    }

    primitive->material = &materials[materialIndex];
    debug("\tMaterial matched: %s", primitiveData->material->name);

    cgltf_accessor    *accessor = primitiveData->indices;
    cgltf_buffer_view *view     = accessor->buffer_view;
    cgltf_buffer      *buffer   = view->buffer;

    void *data = buffer->data + view->offset + accessor->offset;

    primitive->indexBegin    = indexCount;
    primitive->indexCount    = accessor->count;
    primitive->vertexOffset  = vertexCount;
    primitive->uniformOffset = uniformBufferSize;

    debug("\tIndices: %lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            indexBuffer[indexCount + dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        memcpy(&indexBuffer[indexCount], data, view->size);
    } // TODO: Can it be something else?

    cgltf_size primitiveVertexCount = 0;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitiveData->attributes_count; attributeIndex++) {
        cgltf_attribute   *attribute         = &primitiveData->attributes[attributeIndex];

        cgltf_accessor    *attributeAccessor = attribute->data;
        cgltf_buffer_view *attributeView     = attributeAccessor->buffer_view;
        cgltf_buffer      *attributeBuffer   = attributeView->buffer;

        void              *attributeData     = attributeBuffer->data + attributeView->offset + attributeAccessor->offset;

        if(primitiveVertexCount == 0) {
            primitiveVertexCount = attributeAccessor->count;
        }

        assert(primitiveVertexCount == attributeAccessor->count);

        debug("\tAttribute %s: %lu elements of type %lu, total of %lu bytes in size", attribute->name, attributeAccessor->count, attributeAccessor->type, attributeView->size);

        // TODO: Check component data types too
        if(attribute->type == cgltf_attribute_type_position) {
            vec3 *positions = attributeData;

            for(cgltf_size positionIndex = 0; positionIndex < attributeAccessor->count; positionIndex++) {
                memcpy(vertexBuffer[vertexCount + positionIndex].position, positions[positionIndex], sizeof(vec3));
            }
        } else if(attribute->type == cgltf_attribute_type_texcoord) {
            vec2 *texcoords = attributeData;

            for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                memcpy(vertexBuffer[vertexCount + texcoordIndex].texcoord, texcoords[texcoordIndex], sizeof(vec2));
            }
        } // TODO: Load normal and tangent too
    }

    PrimitiveUniform *primitiveUniform = uniformBuffer + primitive->uniformOffset;
    memcpy(primitiveUniform->model, transform, sizeof(primitiveUniform->model));

    debug("\tIndex begin:    %lu", primitive->indexBegin);
    debug("\tIndex count:    %lu", primitive->indexCount);
    debug("\tVertex offset:  %lu", primitive->vertexOffset);
    debug("\tUniform offset: %lu", primitive->uniformOffset);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    indexBufferSize   += accessor->count      * sizeof(Index);
    vertexBufferSize  += primitiveVertexCount * sizeof(Vertex);
    uniformBufferSize += primitiveUniformAlignment;

    primitiveCount++;
}

// NOTICE: This doesn't account for material binding, use bindMaterial() beforehand
void drawPrimitive(VkCommandBuffer commandBuffer, VkDescriptorSet primitiveDescriptorSet, Primitive *primitive) {
    uint32_t primitiveDescriptorOffset = primitive->uniformOffset - sceneUniformAlignment;
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &primitiveDescriptorSet, 1, &primitiveDescriptorOffset);
    vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->indexBegin, primitive->vertexOffset, 0);
}
