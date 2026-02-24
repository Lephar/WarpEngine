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

uint32_t loadPrimitive(cgltf_primitive *primitiveData) {
    assert(primitiveCount < primitiveCountLimit);
    const uint32_t primitiveIndex = primitiveCount;
    primitiveCount++;

    Primitive *primitive = &primitives[primitiveIndex];
    PrimitiveUniform *primitiveUniform = &primitiveUniforms[primitiveIndex];

    debug("Primitive Index: %d", primitiveIndex);
    debug("\tType: %d", primitiveData->type);

    uint32_t materialIndex = findMaterial(primitiveData->material->name);

    if(materialIndex >= materialCount) {
        debug("\tNo material found for the primitive, using the default material...");
        materialIndex = findMaterial(defaultMaterialName);

        if(materialIndex >= materialCount) {
            debug("\tThe default material is not loaded, loading now...");
            loadDefaultMaterial();

            materialIndex = findMaterial(defaultMaterialName);
            assert(materialIndex < materialCount);
        }
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
    primitive->uniformOffset = primitiveIndex * primitiveUniformAlignment;

    debug("\tIndices: %lu elements of type %lu, total of %lu bytes in size", accessor->count, accessor->type, view->size);

    if(accessor->component_type == cgltf_component_type_r_16 || accessor->component_type == cgltf_component_type_r_16u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            indexBuffer[indexCount + dataIndex] = ((uint16_t *) data)[dataIndex];
        }
    } else if(accessor->component_type == cgltf_component_type_r_32u) {
        for(cgltf_size dataIndex = 0; dataIndex < accessor->count; dataIndex++) {
            indexBuffer[indexCount + dataIndex] = ((uint32_t *) data)[dataIndex];
        }
    } // TODO: Can it be something else?

    cgltf_size primitiveVertexCount = 0;

    for(cgltf_size attributeIndex = 0; attributeIndex < primitiveData->attributes_count; attributeIndex++) {
        cgltf_attribute   *attribute         = &primitiveData->attributes[attributeIndex];

        cgltf_accessor    *attributeAccessor = attribute->data;
        cgltf_buffer_view *attributeView     = attributeAccessor->buffer_view;
        cgltf_buffer      *attributeBuffer   = attributeView->buffer;

        void              *attributeData     = attributeBuffer->data + attributeView->offset + attributeAccessor->offset;

        cgltf_size         numComponents     = cgltf_num_components(attributeAccessor->type);

        if(primitiveVertexCount == 0) {
            primitiveVertexCount = attributeAccessor->count;
        }

        assert(primitiveVertexCount == attributeAccessor->count);
        debug("\tAttribute %s:\t%lu elements of type %lu, total of %lu bytes in size", attribute->name, attributeAccessor->count, attributeAccessor->type, attributeView->size);

        // TODO: Check component data types too (Is this necessary?)
        // WARN: mempcy() and glm_vec#_copy() on components cause segfault
        if(attribute->type == cgltf_attribute_type_position) {
            assert(numComponents == 3);
            vec3 *positions = attributeData;

            for(cgltf_size positionIndex = 0; positionIndex < attributeAccessor->count; positionIndex++) {
                for(cgltf_size componentIndex = 0; componentIndex < numComponents; componentIndex++) {
                    vertexBuffer[vertexCount + positionIndex].position[componentIndex] = positions[positionIndex][componentIndex];
                }
                vertexBuffer[vertexCount + positionIndex].position[3] = 1.0f;
            }
        } else if(attribute->type == cgltf_attribute_type_tangent) {
            assert(numComponents == 4);
            vec4 *tangents = attributeData;

            for(cgltf_size tangentIndex = 0; tangentIndex < attributeAccessor->count; tangentIndex++) {
                for(cgltf_size componentIndex = 0; componentIndex < numComponents; componentIndex++) {
                    vertexBuffer[vertexCount + tangentIndex].tangent[componentIndex] = tangents[tangentIndex][componentIndex];
                }
            }
        } else if(attribute->type == cgltf_attribute_type_normal) {
            assert(numComponents == 3);
            vec3 *normals = attributeData;

            for(cgltf_size normalIndex = 0; normalIndex < attributeAccessor->count; normalIndex++) {
                for(cgltf_size componentIndex = 0; componentIndex < numComponents; componentIndex++) {
                    vertexBuffer[vertexCount + normalIndex].normal[componentIndex] = normals[normalIndex][componentIndex];
                }
                vertexBuffer[vertexCount + normalIndex].normal[3] = 0.0f;
            }
        } else if(attribute->type == cgltf_attribute_type_texcoord) {
            assert(numComponents == 2);
            vec2 *texcoords = attributeData;

            if(!strcmp(attribute->name, "TEXCOORD_0")) {
                for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                    for(cgltf_size componentIndex = 0; componentIndex < numComponents; componentIndex++) {
                        vertexBuffer[vertexCount + texcoordIndex].texcoord[componentIndex] = texcoords[texcoordIndex][componentIndex];
                    }
                }
            } else if(!strcmp(attribute->name, "TEXCOORD_1")) {
                for(cgltf_size texcoordIndex = 0; texcoordIndex < attributeAccessor->count; texcoordIndex++) {
                    for(cgltf_size componentIndex = 0; componentIndex < numComponents; componentIndex++) {
                        vertexBuffer[vertexCount + texcoordIndex].texcoord[componentIndex + numComponents] = texcoords[texcoordIndex][componentIndex];
                    }
                }
            }
        } // TODO: Load color and weight too if not too redundant
    }

    debug("\tIndex begin:    %lu", primitive->indexBegin);
    debug("\tIndex count:    %lu", primitive->indexCount);
    debug("\tVertex offset:  %lu", primitive->vertexOffset);
    debug("\tUniform offset: %lu", primitive->uniformOffset);

    glmc_mat4_identity(primitiveUniform->model);
    glmc_mat4_identity(primitiveUniform->normal);

    indexCount  += accessor->count;
    vertexCount += primitiveVertexCount;

    debug("\tBuffer offsets set and primitive created");
    return primitiveIndex;
}

// NOTICE: This doesn't account for material binding, use bindMaterial() beforehand
void drawPrimitive(uint32_t framebufferSetIndex, uint32_t framebufferIndex, Primitive *primitive) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, 1, &framebuffer->primitiveDescriptorSet, 1, &primitive->uniformOffset);
    vkCmdPushConstants(framebuffer->renderCommandBuffer, pipelineLayout, pushConstantRanges[0].stageFlags, pushConstantRanges[0].offset, pushConstantRanges[0].size, &primitive->vertexOffset);
    vkCmdDraw(framebuffer->renderCommandBuffer, primitive->indexCount, 1, primitive->indexBegin, 0);
}
