#include "primitive.h"

#include "pipeline.h"
#include "content.h"
#include "framebuffer.h"
#include "material.h"

#include "logger.h"

uint32_t primitiveCountLimit;
uint32_t primitiveCount;
Primitive *primitives;
PrimitiveUniform *primitiveUniforms;

void loadPrimitive(cgltf_primitive *primitiveData, mat4 transform) {
    assert(primitiveCount < primitiveCountLimit);
    const uint32_t primitiveIndex = primitiveCount;

    Primitive *primitive = &primitives[primitiveIndex];
    PrimitiveUniform *primitiveUniform = &primitiveUniforms[primitiveIndex];

    debug("Primitive Index: %d", primitiveIndex);
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
    primitive->vertexOffset  = (int32_t) vertexCount;
    primitive->uniformOffset = primitiveCount * sizeof(PrimitiveUniform);

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

        debug("\tAttribute %s:\t%lu elements of type %lu, total of %lu bytes in size", attribute->name, attributeAccessor->count, attributeAccessor->type, attributeView->size);

        // TODO: Check component data types too (Is this necessary?)
        if(attribute->type == cgltf_attribute_type_position) {
            vec3 *positions = attributeData;

            for(cgltf_size positionIndex = 0; positionIndex < attributeAccessor->count; positionIndex++) {
                memcpy(vertexBuffer[vertexCount + positionIndex].position, positions[positionIndex], sizeof(vec3));
            }
        } else if(attribute->type == cgltf_attribute_type_tangent) {
            vec4 *tangents = attributeData;

            for(cgltf_size tangentIndex = 0; tangentIndex < attributeAccessor->count; tangentIndex++) {
                // WARN: THERE IS A CLANG BUG ON THE LINE BELOW!
                //memcpy(vertexBuffer[vertexCount + tangentIndex].tangent, tangents[tangentIndex], sizeof(vec4));

                // TODO: Revert this when they fix the issue with clang
                vertexBuffer[vertexCount + tangentIndex].tangent[0] = tangents[tangentIndex][0];
                vertexBuffer[vertexCount + tangentIndex].tangent[1] = tangents[tangentIndex][1];
                vertexBuffer[vertexCount + tangentIndex].tangent[2] = tangents[tangentIndex][2];
                vertexBuffer[vertexCount + tangentIndex].tangent[3] = tangents[tangentIndex][3];
            }
        } else if(attribute->type == cgltf_attribute_type_normal) {
            vec3 *normals = attributeData;

            for(cgltf_size normalIndex = 0; normalIndex < attributeAccessor->count; normalIndex++) {
                memcpy(vertexBuffer[vertexCount + normalIndex].normal, normals[normalIndex], sizeof(vec3));
            }
        } else if(attribute->type == cgltf_attribute_type_texcoord) {
            vec2 *texcoords = attributeData;

            if(!strcmp(attribute->name, "TEXCOORD_0")) {
                for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                    memcpy(vertexBuffer[vertexCount + texcoordIndex].texcoord0, texcoords[texcoordIndex], sizeof(vec2));
                }
            } else if(!strcmp(attribute->name, "TEXCOORD_1")) {
                for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                    memcpy(vertexBuffer[vertexCount + texcoordIndex].texcoord1, texcoords[texcoordIndex], sizeof(vec2));
                }
            }
        } // TODO: Load color and weight if not too redundant
    }

    memcpy(primitiveUniform->model, transform, sizeof(primitiveUniform->model));

    debug("\tIndex begin:    %lu", primitive->indexBegin);
    debug("\tIndex count:    %lu", primitive->indexCount);
    debug("\tVertex offset:  %lu", primitive->vertexOffset);
    debug("\tUniform offset: %lu", primitive->uniformOffset);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    debug("\tBuffer offsets set and primitive created");

    primitiveCount++;
}

// NOTICE: This doesn't account for material binding, use bindMaterial() beforehand
void drawPrimitive(uint32_t framebufferSetIndex, uint32_t framebufferIndex, Primitive *primitive) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &framebuffer->primitiveDescriptorSet, 1, &primitive->uniformOffset);
    vkCmdDrawIndexed(framebuffer->renderCommandBuffer, primitive->indexCount, 1, primitive->indexBegin, primitive->vertexOffset, 0);
}
