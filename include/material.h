#include "pch.h"

#include "helper.h"
#include "image.h"

struct material {
    char name[UINT8_MAX];
    Image baseColor;
    VkDescriptorSet samplerDescriptor;
} typedef Material;

void loadTexture(const char *path, Image *texture);
void createDescriptor(Material *material);
void loadMaterial(cgltf_material *materialData);
