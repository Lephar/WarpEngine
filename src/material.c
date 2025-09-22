#include "material.h"

#include "device.h"
#include "image.h"
#include "pipeline.h"
#include "descriptor.h"
#include "content.h"
#include "framebuffer.h"
#include "texture.h"

#include "logger.h"

Image *defaultBlackTexture = nullptr;
Image *defaultWhiteTexture = nullptr;

const uint32_t materialTextureCount = 3;
uint32_t materialCountLimit;
uint32_t materialCount;
Material *materials;
MaterialUniform *materialUniforms;

uint32_t findMaterial(cgltf_material *materialData) {
    for(uint32_t materialIndex = 0; materialIndex < materialCount; materialIndex++) {
        if(strncmp(materialData->name, materials[materialIndex].name, UINT8_MAX) == 0) {
            return materialIndex;
        }
    }

    return UINT32_MAX;
}

Image *loadUncompressedTexture(const char *subdirectory, const char *filename, bool isColor) {
    PRawTexture rawTexture = initializeRawTexture(subdirectory, filename, isColor);
    loadRawTexture(rawTexture);
    generateRawMipmaps(rawTexture);

    PCompressedTexture compressedTexture = convertRawTexture(rawTexture);
    //generateConvertedMipmaps(compressedTexture);
    compressConvertedTexture(compressedTexture);
    transcodeCompressedTexture(compressedTexture);

    PImage image = createTextureImage(compressedTexture);
    loadTextureImage(image, compressedTexture);

    return image;
}

Image *loadTexture(const char *subdirectory, const char *filename, bool isColor) {
    PCompressedTexture compressedTexture = initializeCompressedTexture(subdirectory, filename, isColor);
    transcodeCompressedTexture(compressedTexture);

    PImage image = createTextureImage(compressedTexture);
    loadTextureImage(image, compressedTexture);

    return image;
}

void loadMaterial(const char *subdirectory,cgltf_material *materialData) {
    assert(materialCount < materialCountLimit);
    const uint32_t materialIndex = materialCount;

    Material        *material        = &materials[materialIndex];
    MaterialUniform *materialUniform = &materialUniforms[materialIndex];

    debug("Material Name: %s", materialData->name);
    strncpy(material->name, materialData->name, UINT8_MAX);

    material->baseColor         = nullptr;
    material->metallicRoughness = nullptr;
    material->normal            = nullptr;

    if(materialData->alpha_mode == cgltf_alpha_mode_blend) {
        material->isTransparent = true;
    } else {
        material->isTransparent = false;
    }

    material->isDoubleSided = materialData->double_sided;

    if(materialData->has_pbr_metallic_roughness) {
        memcpy(materialUniform->baseColorFactor, materialData->pbr_metallic_roughness.base_color_factor, sizeof(vec4));
        debug("\tBase color factor: [%0.4f, %0.4f, %0.4f, %0.4f]", materialUniform->baseColorFactor[0], materialUniform->baseColorFactor[1], materialUniform->baseColorFactor[2], materialUniform->baseColorFactor[3]);

        if(materialData->pbr_metallic_roughness.base_color_texture.texture) {
            if(materialData->pbr_metallic_roughness.base_color_texture.texture->has_basisu) {
                material->baseColor = loadTexture(subdirectory, materialData->pbr_metallic_roughness.base_color_texture.texture->basisu_image->uri, true);
            } else {
                material->baseColor = loadUncompressedTexture(subdirectory, materialData->pbr_metallic_roughness.base_color_texture.texture->image->uri, true);
            }
        } else {
            if(defaultWhiteTexture == nullptr) {
                defaultWhiteTexture = loadTexture("assets/default/textures", "white.ktx2", true);
            }

            material->baseColor = defaultWhiteTexture;
        }

        materialUniform->metallicRoughnessFactor[0] = materialData->pbr_metallic_roughness.metallic_factor;
        materialUniform->metallicRoughnessFactor[1] = materialData->pbr_metallic_roughness.roughness_factor;
        debug("\tMetallic roughness factor: [%0.4f, %0.4f]", materialUniform->metallicRoughnessFactor[0], materialUniform->metallicRoughnessFactor[1]);

        if(materialData->pbr_metallic_roughness.metallic_roughness_texture.texture) {
            if(materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->has_basisu) {
                material->metallicRoughness = loadTexture(subdirectory, materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->basisu_image->uri, false);
            } else {
                material->metallicRoughness = loadUncompressedTexture(subdirectory, materialData->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri, false);
            }
        } else {
            if(defaultBlackTexture == nullptr) {
                defaultBlackTexture = loadTexture("assets/default/textures", "black.ktx2", true);
            }

            material->metallicRoughness = defaultBlackTexture;
        }
    }

    if(materialData->normal_texture.texture) {
        materialUniform->normalScale = materialData->normal_texture.scale;
        debug("\tNormal scale: %0.4f", materialUniform->normalScale);

        if(materialData->normal_texture.texture->has_basisu) {
            material->normal = loadTexture(subdirectory, materialData->normal_texture.texture->basisu_image->uri, false);
        } else {
            material->normal = loadUncompressedTexture(subdirectory, materialData->normal_texture.texture->image->uri, false);
        }
    } else {
        if(defaultBlackTexture == nullptr) {
            defaultBlackTexture = loadTexture("assets/default/textures", "black.ktx2", true);
        }

        material->normal = defaultBlackTexture;
    }

    memcpy(materialUniform->emissiveFactor, materialData->emissive_factor, sizeof(vec3));
    debug("\tEmissive factor: [%0.4f, %0.4f, %0.4f]", materialUniform->emissiveFactor[0], materialUniform->emissiveFactor[1], materialUniform->emissiveFactor[2]);

    material->factorOffset = materialIndex * materialUniformAlignment;
    material->samplerDescriptorSet = getSamplerDescriptorSet(material);

    debug("\tTexture descriptor sets acquired and material created");

    materialCount++;
}

// NOTICE: This doesn't account for shader binding, use bindShader() beforehand
void bindMaterial(uint32_t framebufferSetIndex, uint32_t framebufferIndex, Material *material) {
    FramebufferSet *framebufferSet = &framebufferSets[framebufferSetIndex];
    Framebuffer *framebuffer = &framebufferSet->framebuffers[framebufferIndex];

    VkColorBlendEquationEXT colorBlendEquations = {
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    VkDescriptorSet descriptorSets[] = {
        framebuffer->materialDescriptorSet,
        material->samplerDescriptorSet,
    };

    uint32_t descriptorSetCount = sizeof(descriptorSets) / sizeof(VkDescriptorSet);

    PFN_vkCmdSetColorBlendEnableEXT cmdSetColorBlendEnable = loadDeviceFunction("vkCmdSetColorBlendEnableEXT");
    cmdSetColorBlendEnable(framebuffer->renderCommandBuffer, 0, 1, &material->isTransparent);
    PFN_vkCmdSetColorBlendEquationEXT cmdSetColorBlendEquation = loadDeviceFunction("vkCmdSetColorBlendEquationEXT");
    cmdSetColorBlendEquation(framebuffer->renderCommandBuffer, 0, 1, &colorBlendEquations);

    vkCmdBindDescriptorSets(framebuffer->renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, descriptorSetCount, descriptorSets, 1, &material->factorOffset);
}

void destroyMaterial(Material *material) {
    if(material->normal && material->normal != defaultBlackTexture) {
        destroyImageView(material->normal);
        destroyImage(material->normal);
    }

    if(material->metallicRoughness && material->metallicRoughness != defaultBlackTexture) {
        destroyImageView(material->metallicRoughness);
        destroyImage(material->metallicRoughness);
    }

    if(material->baseColor != defaultWhiteTexture) {
        destroyImageView(material->baseColor);
        destroyImage(material->baseColor);
    }
}
